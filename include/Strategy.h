#ifndef STRATEGY_H
#define STRATEGY_H


#include <L3OrderBook.h>
#include <L3OrderBookListener.h>


namespace sob {


template <template <typename T, typename AllocT=std::allocator<T> > class BuffType>
class LoggingStrategy
    : public L3OrderBookListener<BuffType>
{

public:
    virtual void onBookUpdate( L3Book<BuffType>* book, const Order& order )
    {
        if (order.isCancel()) {
            spdlog::info( "[LoggingStrategy] Order Cancelled: {}", order.toString() );
        }
        else if (order.isReprice()) {
            spdlog::info( "[LoggingStrategy] Order Modified: {}", order.toString() );
        }
        else {
            spdlog::info( "[LoggingStrategy] Order Added: {}", order.toString() );
        }
    }

    virtual void onTradeMsg( L3Book<BuffType>* book, const Trade& trade )
    {
        spdlog::info( "[LoggingStrategy] Trade Received: {}", trade.toString() );
    }

    virtual void onSnapShotMsg( L3Book<BuffType>* book, L2Book& snapshot )
    {
        spdlog::info( "[LoggingStrategy] Snapshot Received: {}", snapshot.toString() );
    }
}; // class Strategy


} // namespace sob

#endif
