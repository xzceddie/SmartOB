#ifndef SMART_ORDER_BOOK_H
#define SMART_ORDER_BOOK_H


#include <L3OrderBook.h>
#include <L3OrderBookListener.h>



namespace sob {


template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;


template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class Synchronizer
    : public L3OrderBookListener<BuffType>
{
private:
    int actualOrderCnt{};
    int actualTradeCnt{};
    int receivedTradeCnt{};
    int receivedSnapShotCnt{};

public:
    Synchronizer() = default;

    Synchronizer( std::shared_ptr<L3Book<BuffType>> book )
    : L3OrderBookListener<BuffType>( book )
    {
    }
    
    virtual void onBookUpdate( std::shared_ptr<L3Book<BuffType>>& book, const Order& order ) override
    {
        actualOrderCnt++;
        if (book -> isAggressive( order )) {
            actualTradeCnt++;
        }
    }
}; // class Synchronizer


/**
 *  @brief This is used to gather the latest information received and produce a most up-to-date order book
 *  @NOTE  The result contain some guesses/deductions,
 *  @NOTE  This is not a Template class;
 */
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class SmartOrderBook
{
private:
    L3Book<BuffType> bookGroundTruth;
    L3Book<BuffType> bookTrade;
    L3Book<BuffType> bookSnapShot;

    Synchronizer<BuffType> synchronizer;
}; // class SmartOrderBook




} // namespace sob





#endif
