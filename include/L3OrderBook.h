#ifndef L3_ORDERBOOK_H
#define L3_ORDERBOOK_H


#include <unordered_map>
#include <OrderBook.h>



namespace sob {

template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;

/**
 *  @brief  This is not a Template class;
 */
class L3Book
    // : public L2Book
{
private:
    OneSideBook<L3PriceLevel, BidComparator> bidBook;
    OneSideBook<L3PriceLevel, AskComparator> askBook;
    std::unordered_map<int, std::list<Order>::iterator> orderMap;

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

    OneSideBook<L3PriceLevel, BidComparator>& getBidSide()
    {
        return bidBook;
    }

    OneSideBook<L3PriceLevel, AskComparator>& getAskSide()
    {
        return askBook;
    }

    L3PriceLevel getBestBidL3() const
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

    L3PriceLevel getBestAskL3() const
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

    std::optional<std::list<Order>::iterator> queryOrderId( const int orderId ) const
    {
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
        assert( !order.isReprice() );

        if( order.isSell ) {
            if ( bidBook.empty() || order.price > getBestBid().price ) {
                // Not aggressive/cross/market Order, quote orders only
                askSideSize += order.size;
                if ( askBook.find( order.price ) == askBook.end() ) {
                    askBook[order.price] = L3PriceLevel{ std::vector<Order>{ order } };
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
                        // while (true) {
                        //     auto& front_order = it->second.orders.front();
                        //     if( order.size > front_order.size ) {
                        //         const auto poped_id = it->second.orders.front().orderId;
                        //         order.size -= front_order.size;
                        //         it->second.orders.pop_front();
                        //         orderMap.erase( poped_id );
                        //     } else {
                        //         it->second.orders.front().size -= order.size;
                        //         break;
                        //     }
                        // }
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
        return false;
    }

    // TODO: Implement cancelOrder
    // true: cancelled, false: cancel fail
    virtual bool cancelOrder( const Order& order )
    {
        return false;
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
