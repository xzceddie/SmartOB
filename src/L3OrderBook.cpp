#include <L3OrderBook.h>
#include <common.h>
#include <L3OrderBookListener.h>



namespace sob {


// true: aggressive, false: not aggressive
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType>
bool L3Book<BuffType>::newOrder( Order& order )
{
    assert( !order.isCancel() );
    // assert( !order.isReprice() );

    // notify listeners before book has updated
    if( ( !order.isReprice() ) && ( !order.isCancel() ) ) {
        for( auto& listener: listeners ) {
            listener->onBookUpdate( this, order );
        }
    }

    if( order.isSell ) {
        if ( bidBook.empty() || order.price > getBestBid().price ) {
            // Not aggressive/cross/market Order, quote orders only
            askSideSize += order.size;
            if ( askBook.find( order.price ) == askBook.end() ) {
                askBook[order.price] = L3PriceLevel<BuffType>{ std::vector<Order>{ order } };
                orderMap[ order.orderId ] = askBook[order.price].orders.begin();
            } else {
                auto it = askBook[order.price].addNewOrder( order, orderMap );
                orderMap[order.orderId] = it;
            }
            return false;
        } else {
            // aggressive order
            for( auto it = bidBook.begin(); it != bidBook.end(); ) {
                const auto best_bid_size = getBestBid().quantity;
                if( it->first < order.price ) {
                    askBook[order.price] = L3PriceLevel<BuffType>{ std::vector<Order>{ order } };
                    orderMap[order.orderId] = askBook[order.price].orders.begin();
                    askSideSize += order.size;
                    return true;
                } 
                
                if( best_bid_size <= order.size ) { // this level will be filled
                    order.size -= best_bid_size;
                    for( auto oit = it->second.orders.begin(); oit != it->second.orders.end(); ++oit ) {
                        orderMap.erase( oit->orderId );
                    }
                    it = bidBook.erase(it);
                    bidSideSize -= best_bid_size;
                } else {    // this level will not be filled
                    bidSideSize -= ( it->second ).matchOrder( order, orderMap );
                    return true;
                }
            }
            // this order has consumed all bidBook
            askBook[ order.price ] = L3PriceLevel<BuffType>{ std::vector<Order>{ order } };
            orderMap[order.orderId] = askBook[order.price].orders.begin();
            askSideSize += order.size;
            return true;
        }
    } else {
        if ( askBook.empty() || order.price < getBestAsk().price ) {
            // Not aggressive/cross/market Order, quote orders only
            bidSideSize += order.size;
            if ( bidBook.find( order.price ) == bidBook.end() ) {
                bidBook[order.price] = L3PriceLevel<BuffType>{ std::vector<Order>{ order } };
                orderMap[ order.orderId ] = bidBook[order.price].orders.begin();
            } else {
                auto it = bidBook[order.price].addNewOrder( order, orderMap );
                orderMap[ order.orderId ] = it;
            }
            return false;
        } else {
            // aggressive order
            for( auto it = askBook.begin(); it != askBook.end(); ) {
                const auto best_ask_size = getBestAsk().quantity;
                if( it->first > order.price ) {
                    bidBook[order.price] = L3PriceLevel<BuffType>{ std::vector<Order>{ order } };
                    orderMap[order.orderId] = bidBook[order.price].orders.begin();
                    bidSideSize += order.size;
                    return true;
                } 
                
                if( best_ask_size <= order.size ) { // this level will be filled
                    order.size -= best_ask_size;
                    for( auto oit = it->second.orders.begin(); oit != it->second.orders.end(); ++oit ) {
                        orderMap.erase( oit->orderId );
                    }
                    it = askBook.erase(it);
                    askSideSize -= best_ask_size;
                } else {    // this level will not be filled
                    askSideSize -= ( it->second ).matchOrder( order, orderMap );
                    return true;
                }
            }
            // this order has consumed all askBook
            bidBook[ order.price ] = L3PriceLevel<BuffType>{ std::vector<Order>{ order } };
            orderMap[order.orderId] = bidBook[order.price].orders.begin();
            bidSideSize += order.size;
            return true;
        }
    }
}


// true: reprice success, false: reprice fail
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType>
bool L3Book<BuffType>::modifyOrder( const Order& order )
{
    if ( (!order.isReprice()) || ( orderMap.find( *order.oldId ) == orderMap.end() ) ) {
        return false;
    }

    // notify listeners before book has updated
    if( ( order.isReprice() ) ) {
        for( auto& listener: listeners ) {
            listener->onBookUpdate( this, order );
        }
    }

    auto oldOrder = *(orderMap[ *order.oldId ]);
    const auto canceled = cancelId( oldOrder.orderId );

    if( !canceled ) {
        return false;   // old order not found, refuse to reprice, maybe it has been traded
    } else {
        Order new_order = order;
        newOrder( new_order );
        return true;
    }
}

// true: cancelled, false: cancel fail
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType>
bool L3Book<BuffType>::cancelOrder( const Order& order )
{
    if (!order.isCancel()) {
        return false;
    }

    // notify listeners before book has updated
    if( ( order.isCancel() ) ) {
        for( auto& listener: listeners ) {
            listener->onBookUpdate( this, order );
        }
    }
    return cancelId(*order.oldId );
}


template class L3Book<std::list>;
template class L3Book<boost::circular_buffer>;


} // namespace sob

