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

public:
    Synchronizer() = default;

    Synchronizer( std::shared_ptr<L3Book<BuffType>> book )
    : L3OrderBookListener<BuffType>( book )
    {}
    
    virtual SyncMode onBookUpdate( std::shared_ptr<L3Book<BuffType>>& book, const Order& order ) override
    {
        actualOrderCnt++;
        actualSnapShotCnt++;
        
        if (book -> isAggressive( order )) {
            actualTradeCnt++;
        }

        if( actualOrderCnt > actualSnapShotCnt + 1 && actualTradeCnt > actualSnapShotCnt + 1 ) {
            return SyncMode::ORDER_IN_LEAD;
        }
        return SyncMode::SYNCHRONOUS;
    }
    
    virtual SyncMode onTradeMsg( std::shared_ptr<L3Book<BuffType>>& book, const Trade& trade ) override
    {
        receivedTradeCnt++;
        if( actualTradeCnt < receivedTradeCnt ) {
            return SyncMode::TRADE_IN_LEAD;
        }
        return SyncMode::SYNCHRONOUS;
    }

    virtual SyncMode onSnapShotMsg( std::shared_ptr<L3Book<BuffType>>& book ) override
    {
        receivedSnapShotCnt++;
        if( actualSnapShotCnt < actualSnapShotCnt ) {
            return SyncMode::SNAPSHOT_IN_LEAD;
        }
        return SyncMode::SYNCHRONOUS;
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
    L3Book<BuffType> bookGroundTruth;
    L3Book<BuffType> bookTrade;
    L3Book<BuffType> bookSnapShot;

    Synchronizer<BuffType> synchronizer;

    bool doGuess{ true };
}; // class SmartOrderBook




} // namespace sob





#endif
