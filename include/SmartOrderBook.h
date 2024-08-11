#ifndef SMART_ORDER_BOOK_H
#define SMART_ORDER_BOOK_H


#include <L3OrderBook.h>
#include <L3OrderBookListener.h>



namespace sob {


template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;


enum class SyncMode
{
    SYNCHRONOUS,
    ORDER_IN_LEAD,
    TRADE_IN_LEAD,
    SNAPSHOT_IN_LEAD
}; // enum class SyncMode

template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class Synchronizer
    : public L3OrderBookListener<BuffType>
{
private:
    int actualOrderCnt{};
    int actualTradeCnt{};
    int actualSnapShotCnt{};
    int receivedTradeCnt{};
    int receivedSnapShotCnt{};

    SyncMode mode = SyncMode::SYNCHRONOUS; // this stores the most up-to-date synchronous status
    SyncMode lastMode = SyncMode::SYNCHRONOUS;

public:
    Synchronizer() = default;

    Synchronizer( std::shared_ptr<L3Book<BuffType>> book )
    : L3OrderBookListener<BuffType>( book )
    {}

    SyncMode getSyncStatus() const
    {
        return mode;
    }

    SyncMode getLastSyncStatus() const
    {
        return lastMode;
    }
    
    virtual void onBookUpdate( L3Book<BuffType>* book, const Order& order ) override
    {
        actualOrderCnt++;
        actualSnapShotCnt++;
        
        if (book -> isAggressive( order )) {
            actualTradeCnt++;
        }

        if( actualOrderCnt > actualSnapShotCnt + 1 && actualTradeCnt > actualSnapShotCnt + 1 ) {
            // return mode = SyncMode::ORDER_IN_LEAD;
            mode = SyncMode::ORDER_IN_LEAD;
        }
        // return mode = SyncMode::SYNCHRONOUS;
        mode = SyncMode::SYNCHRONOUS;
    }
    
    virtual void onTradeMsg( L3Book<BuffType>* book, const Trade& trade ) override
    {
        receivedTradeCnt++;
        if( actualTradeCnt < receivedTradeCnt ) {
            // return mode = SyncMode::TRADE_IN_LEAD;
            mode = SyncMode::TRADE_IN_LEAD;
        }
        // return mode = SyncMode::SYNCHRONOUS;
        mode = SyncMode::SYNCHRONOUS;
    }

    virtual void onSnapShotMsg( L3Book<BuffType>* book ) override
    {
        receivedSnapShotCnt++;
        if( actualSnapShotCnt < actualSnapShotCnt ) {
            // return mode = SyncMode::SNAPSHOT_IN_LEAD;
            mode = SyncMode::SNAPSHOT_IN_LEAD;
        }
        // return mode = SyncMode::SYNCHRONOUS;
        mode = SyncMode::SYNCHRONOUS;
    }
}; // class Synchronizer


/**
 *  @brief This is used to gather the latest information received and produce a most up-to-date order book
 *  @NOTE  The result contain some guesses/deductions,
 *  @NOTE  This is not a Template class;
 *  @member doGuess: if false, only reflect the information carried by the orderstream, else do the guess ASAP, default to true
 */
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class SmartOrderBook
{
private:
    std::shared_ptr<L3Book<BuffType>> bookGroundTruth;
    std::shared_ptr<L3Book<BuffType>> bookTrade;
    std::shared_ptr<L3Book<BuffType>> bookSnapShot;

    Synchronizer<BuffType> synchronizer;

    bool doGuess{ true };

    std::shared_ptr<L3Book<BuffType>> leaderBook;

public:
    // synchronizer should subscribe to all three books
    SmartOrderBook()
    {
        synchronizer.subscribe( bookGroundTruth );
        synchronizer.subscribe( bookTrade );
        synchronizer.subscribe( bookSnapShot );
    }

    auto applyOrder( Order& order )
    {
        return bookGroundTruth->applyOrder( order );
    }

    auto applyTrade( Trade& trade )
    {
        return bookTrade->applyTrade( trade );
    }

    auto applySnapShot( L2Book& snapshot )
    {
        return bookSnapShot->applySnapShot( snapshot );
    }

    
    void applyMessage( const std::string& str )
    {
        if( doGuess ) {
            if ( str[0] == 'N' || str[0] == 'C' || str[0] == 'C' ) {
                Order order{ str };
                applyOrder( order );
            } else if ( str[0] == 'T' ) {
                Trade trade{ str };
                applyTrade( trade );
            } else if ( str[0] == 'S' ) {
                L2Book snapshot{ str };
                applySnapShot( snapshot );
            }

            auto status = synchronizer.getSyncStatus();
            auto last_status = synchronizer.getLastSyncStatus();

            {
                /**
                 *  @brief  when it is now anything but ORDER_IN_LEAD and last status is ORDER_IN_LEAD,
                 *              update bookTrade and bookSnapShot with the ground truth
                 */
                if (status == SyncMode::SYNCHRONOUS) {
                    leaderBook = bookGroundTruth;
                    // TODO: we should make it asynchronous, i.e. in another thread such that it will not block the main logic
                    if( last_status == SyncMode::ORDER_IN_LEAD ) {
                        *bookTrade = *bookGroundTruth;
                        *bookSnapShot = *bookGroundTruth;
                    }
                }

                else if( status == SyncMode::ORDER_IN_LEAD ) {
                    leaderBook = bookGroundTruth;
                }

                else if( status == SyncMode::TRADE_IN_LEAD ) {
                    leaderBook = bookTrade;
                    if( last_status == SyncMode::ORDER_IN_LEAD ) {
                        *bookTrade = *bookGroundTruth;
                        *bookSnapShot = *bookGroundTruth;
                    }
                }

                else if( status == SyncMode::SNAPSHOT_IN_LEAD ) {
                    leaderBook = bookSnapShot;
                    if( last_status == SyncMode::ORDER_IN_LEAD ) {
                        *bookTrade = *bookGroundTruth;
                        *bookSnapShot = *bookGroundTruth;
                    }
                }
            }
            
        } else {
            if ( str[0] == 'N' || str[0] == 'C' || str[0] == 'C' ) {
                Order order{ str };
                applyOrder( order );
            }
        }
    }

    std::shared_ptr<L3Book<BuffType>> getLeaderBook() const
    {
        // noGuess: always return the most accurate book, i.e., bookGroundTruth
        if ( !doGuess ) {
            return bookGroundTruth;
        }
        return leaderBook;
    }
}; // class SmartOrderBook


} // namespace sob





#endif
