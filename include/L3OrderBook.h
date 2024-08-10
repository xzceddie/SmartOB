#ifndef L3_ORDERBOOK_H
#define L3_ORDERBOOK_H


#include <unordered_map>
#include <OrderBook.h>
#include <common.h>
#include <dBuffer.h>



namespace sob {

template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;

/**
 *  @brief  This is not a Template class;
 */
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class L3Book
    // : public L2Book
{
private:
    OneSideBook<L3PriceLevel<BuffType>, BidComparator> bidBook;
    OneSideBook<L3PriceLevel<BuffType>, AskComparator> askBook;
    // std::unordered_map<int, std::list<Order>::iterator> orderMap;
    std::unordered_map<int, dBuffer<Order, boost::circular_buffer>::iterator> orderMap;

    size_t bidSideSize = 0;
    size_t askSideSize = 0;

public:
    size_t getBidSideSize() const
    {
        return bidSideSize;
    }

    size_t getAskSideSize() const
    {
        return askSideSize;
    }

    size_t getBidSideDepth() const
    {
        return bidBook.size();
    }

    size_t getAskSideDepth() const
    {
        return askBook.size();
    }

    OneSideBook<L3PriceLevel<BuffType>, BidComparator>& getBidSide()
    {
        return bidBook;
    }

    OneSideBook<L3PriceLevel<BuffType>, AskComparator>& getAskSide()
    {
        return askBook;
    }

    L3PriceLevel<BuffType> getBestBidL3() const
    {
        if ( bidBook.empty() ) {
            return L3PriceLevel{};
        }
        return bidBook.begin()->second;
    }

    L2PriceLevel getBestBid() const
    {
        if ( bidBook.empty() ) {
            return L2PriceLevel{};
        }
        return bidBook.begin()->second.toL2PriceLevel();
    }

    L3PriceLevel<BuffType> getBestAskL3() const
    {
        if ( askBook.empty() ) {
            return L3PriceLevel{};
        }
        return askBook.begin()->second;
    }

    L2PriceLevel getBestAsk() const
    {
        if ( askBook.empty() ) {
            return L2PriceLevel{};
        }
        return askBook.begin()->second.toL2PriceLevel();
    }

    void prtOrderMap()
    {
        std::cout << "------ start of order ids: ------\n";
        for(auto&[ order_id, order_it]: orderMap) {
            std::cout << order_id << ": " << *order_it << std::endl;
        }
        std::cout << "------  end of order ids   ------\n";
    }



    // std::optional<std::list<Order>::iterator> queryOrderId( const int orderId ) const
    std::optional<typename BuffType<Order>::iterator> queryOrderId( const int orderId ) const
    {
#ifdef DEBUG_ORDER_MAP
        prtOrderMap();
#endif
        if ( orderMap.find( orderId ) == orderMap.end() ) {
            return {};
        }
        return orderMap.find( orderId )->second;
    }

public:
    L3Book() = default;

    // true: aggressive, false: not aggressive
    virtual bool newOrder( Order& order )
    {
        assert( !order.isCancel() );
        // assert( !order.isReprice() );

        if( order.isSell ) {
            if ( bidBook.empty() || order.price > getBestBid().price ) {
                // Not aggressive/cross/market Order, quote orders only
                askSideSize += order.size;
                if ( askBook.find( order.price ) == askBook.end() ) {
                    askBook[order.price] = L3PriceLevel{ std::vector<Order>{ order } };
                    orderMap[ order.orderId ] = askBook[order.price].orders.begin();
                } else {
                    auto it = askBook[order.price].addNewOrder( order );
                    orderMap[order.orderId] = it;
                }
                return false;
            } else {
                // aggressive order
                for( auto it = bidBook.begin(); it != bidBook.end(); ) {
                    const auto best_bid_size = getBestBid().quantity;
                    if( it->first < order.price ) {
                        askBook[order.price] = L3PriceLevel{ std::vector<Order>{ order } };
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
                askBook[ order.price ] = L3PriceLevel{ std::vector<Order>{ order } };
                orderMap[order.orderId] = askBook[order.price].orders.begin();
                askSideSize += order.size;
                return true;
            }
        } else {
            if ( askBook.empty() || order.price < getBestAsk().price ) {
                // Not aggressive/cross/market Order, quote orders only
                bidSideSize += order.size;
                if ( bidBook.find( order.price ) == bidBook.end() ) {
                    bidBook[order.price] = L3PriceLevel{ std::vector<Order>{ order }};
                    orderMap[ order.orderId ] = bidBook[order.price].orders.begin();
                } else {
                    auto it = bidBook[order.price].addNewOrder( order );
                    orderMap[ order.orderId ] = it;
                }
                return false;
            } else {
                // aggressive order
                for( auto it = askBook.begin(); it != askBook.end(); ) {
                    const auto best_ask_size = getBestAsk().quantity;
                    if( it->first > order.price ) {
                        bidBook[order.price] = L3PriceLevel{ std::vector<Order>{ order } };
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
                bidBook[ order.price ] = L3PriceLevel{ std::vector<Order>{ order } };
                orderMap[order.orderId] = bidBook[order.price].orders.begin();
                bidSideSize += order.size;
                return true;
            }
        }
    }

    // TODO: Implement modifyOrder
    // true: reprice success, false: reprice fail
    virtual bool modifyOrder( const Order& order )
    {
        if ( (!order.isReprice()) || ( orderMap.find( *order.oldId ) == orderMap.end() ) ) {
            return false;
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

    virtual bool cancelId( const int id )
    {
        if( orderMap.find( id ) == orderMap.end() ) {
            return false;
        }
        auto it = orderMap[id];
        if ((*it).isSell) {
            auto& L3Level = askBook[ (*it).price ];
            askSideSize -= it->size;
            L3Level.numOrders--;
            L3Level.quantity -= it->size;
            L3Level.orders.erase( it );
        } else{
            auto& L3Level = bidBook[ (*it).price ];
            bidSideSize -= it->size;
            L3Level.numOrders--;
            L3Level.quantity -= it->size;
            L3Level.orders.erase( it );
        }
        orderMap.erase( id );
        return true;
    }

    // TODO: Implement cancelOrder
    // true: cancelled, false: cancel fail
    virtual bool cancelOrder( const Order& order )
    {
        if (!order.isCancel()) {
            return false;
        }
        return cancelId(*order.oldId );
    }
 

    L3Book( std::vector<Order>& orders )
    {
        for(auto& order: orders)
        {
            newOrder( order );
        }
    }


    auto getBestMarketL3() const
    {
        L3PxLvlPair ret;
        if ( !askBook.empty() ) {
            ret.ask = askBook.begin()->second;
        }
        if ( !bidBook.empty() ) {
            ret.bid = bidBook.begin()->second;
        }
        return ret;
    }
    
    // to keep it the same interface as L2Book
    auto getBestMarket() const
    {
        return getBestMarketL3().toL2PxLvlPair();
    }


    std::string toString() const
    {
        std::stringstream ss;

        for( auto rit = askBook.rbegin(); rit != askBook.rend(); ++rit ) {
            ss << rit->second.toString() << '\n';
        }

        ss << "------\n";

        for( auto it = bidBook.begin(); it != bidBook.end(); ++it ) {
            ss << it->second.toString() << '\n';
        }

        return ss.str();
    }
}; // class L3Book


} // namespace sob


#endif
