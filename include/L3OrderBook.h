#ifndef L3_ORDERBOOK_H
#define L3_ORDERBOOK_H


#include <unordered_map>
#include <OrderBook.h>
#include <common.h>
#include <dBuffer.h>
#include <Trade.h>
#include <unordered_map>
#include <spdlog/fmt/fmt.h>
#include <IdGen.h>



namespace sob {


template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class L3OrderBookListener;

template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;

/**
 *  @brief  An Orderbook that keeps information on all levels plus the order information
 *          BuffType is the type of buffer used to hold all the orders on one level
 *  @NOTE   This is not a Template class;
 */
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class L3Book
    // : public L2Book
{
private:
    OneSideBook<L3PriceLevel<BuffType>, BidComparator> bidBook;
    OneSideBook<L3PriceLevel<BuffType>, AskComparator> askBook;
    std::unordered_map<int, typename dBuffer<Order, BuffType>::iterator> orderMap;

    size_t bidSideSize = 0;
    size_t askSideSize = 0;

    std::vector<std::shared_ptr<L3OrderBookListener<BuffType>>> listeners;

public:

    /**
     * @brief   a custom copy constructor is necessary to deal with the iterator problem
     *           trivially copied, the copied iterators will point to the original object
     *
     * @NOTE    This function will be O(N^2) for std::list but only O(N) for boost::circular_buffer, N is the number of orders in the whole L3Book
     *          We shall use it cautiously
     */
    L3Book( const L3Book<BuffType>& rhs )
    : bidBook { rhs.bidBook }
    , askBook { rhs.askBook }
    , bidSideSize { rhs.bidSideSize }
    , askSideSize { rhs.askSideSize }
    {
        for( auto&[ ind, it ]: rhs.orderMap ) {
            const double px = it->price;
            
            if (it -> isSell) {
                // orderMap[ind] = askBook[px].orders.begin() + ( it - const_cast<typename BuffType<Order>::iterator>(rhs.askBook.at(px).orders.begin()) );
                orderMap[ind] = std::next(askBook[px].orders.begin(), std::distance( static_cast<typename BuffType<Order>::iterator>(it), rhs.askBook.at(px).orders.begin()));
            } else {
                // orderMap[ind] = bidBook[px].orders.begin() + ( it - const_cast<typename BuffType<Order>::iterator>(rhs.bidBook.at(px).orders.begin()) );
                // orderMap[ind] = bidBook[px].orders.begin() + ( static_cast<typename BuffType<Order>::iterator>(it) - rhs.bidBook.at(px).orders.begin());
                orderMap[ind] = std::next(bidBook[px].orders.begin(), std::distance( static_cast<typename BuffType<Order>::iterator>(it), rhs.bidBook.at(px).orders.begin()));
            }
        }
    }

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
            return L3PriceLevel<BuffType>{};
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
            return L3PriceLevel<BuffType>{};
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


    void accept( L3OrderBookListener<BuffType>* listener )
    {
        listeners.push_back( std::shared_ptr<L3OrderBookListener<BuffType>>( listener ) );
    }

    bool isAggressive( const Order& order ) const
    {
        if ( order.isSell ) {
            if ( bidBook.empty() || order.price > getBestBid().price ) {
                return false;
            }
            return true;
        } else {
            if ( askBook.empty() || order.price < getBestAsk().price ) {
                return false;
            }
            return true;
        }
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
    bool newOrder( Order& order )
    {
        assert( !order.isCancel() );
        // assert( !order.isReprice() );

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
    bool modifyOrder( const Order& order )
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

    // TODO: change the L3Level.orders.erase(it) to something better, i.e. mark an order is canceled
    bool cancelId( const int id )
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

    // true: cancelled, false: cancel fail
    bool cancelOrder( const Order& order )
    {
        if (!order.isCancel()) {
            return false;
        }
        return cancelId(*order.oldId );
    }


    /**
     *  @brief  apply order of either of the type OrderType::Normal, OrderType::Cancel, OrderType::Reprice
     */
    std::pair<OrderType, bool> applyOrder( Order& order )
    {
        switch (order.getType())
        {
            case OrderType::Normal:
                return { OrderType::Normal, newOrder( order ) };
            case OrderType::Cancel:
                return { OrderType::Cancel, cancelOrder( order ) };
            case OrderType::Reprice:
                return { OrderType::Reprice, modifyOrder( order ) };
            default:
                throw std::runtime_error( "unknown order type" );
        }
    }

    /**
     *  @brief  Happens when trade stream leads L3 Order Stream and L2 Snapshot Stream
     */
    void applyUnseenTrade( Trade& trade )
    {
        if( trade.isSell ) {
            // Seller is the liquidity taker
            const auto best_bid_px = getBestBid().price;
            const int lvl_cnt = trade.getLvlCnt();

            if (lvl_cnt == 1) {
                const auto trd_px = trade.price[0];
                
                if( trd_px > best_bid_px ) {
                    /* *******************************************
                     * There has to be some new bid orders at the trd_px which haven't arrived
                     * Then come this trade, initiated by some selling liquidity taker
                     *
                     * !!! There is most likely some liquidity left on the traded lvl
                     * !!! We can't have an accurate guess about how much liquidity there is left on that level, 
                     * !!! therefore we hold on the guess until that level has been totally consumed
                     * *******************************************/
                    static std::unordered_map<double, int> recored_trds_vol;
                    static double last_unconsumed_lvl = 0;

                    if ( trd_px == last_unconsumed_lvl ) {
                        recored_trds_vol[trd_px] += trade.getTotalVol();
                    } else {
                        /* *******************************************
                         * !!! Now the last_unconsumed_lvl has bee consumed;;
                         * !!! We may now make a guess:
                         *          That price level is now on the ask side
                         *          The guessed ask side size is: 2 * ( recored_trds_vol[trd_px] + trade.getTotalVol() )
                         *          For simplicity, we just guess that there is one order there
                         * *******************************************/
                        newOrder( Order( fmt::format("N {} 1 {} {}", 
                                                     IdGenNeg::getInstance().genId(),
                                                     2 * recored_trds_vol[trd_px],
                                                     last_unconsumed_lvl ) ) );
                        recored_trds_vol.erase( last_unconsumed_lvl );
                        last_unconsumed_lvl = trd_px;
                    }
                } else if ( trd_px == best_bid_px ) {
                    /* *******************************************
                     * This has been a very logical trade message, we should just reflect the information in the orderbook
                     *  NOTE: I'm simplifying this logic, to only estimate the untraded quantity using the last trade mesage
                     *        The best level
                     * *******************************************/
                    const auto trd_qty = trade.getTotalVol();
                    if ( trd_qty < getBestBid().quantity ) {
                        // simply reflect the trade information on the book
                        newOrder( Order( fmt::format("N {} 1 {} {}", 
                                                     IdGenNeg::getInstance().genId(),
                                                     trd_qty,
                                                     best_bid_px ) ) );
                    } else {
                        // trade quantity + not yet received aggresive orders
                        newOrder( Order( fmt::format("N {} 1 {} {}", 
                                                     IdGenNeg::getInstance().genId(),
                                                     3 * trd_qty,
                                                     best_bid_px ) ) );
                    }
                    
                } else {
                    // throw std::runtime_error( "unexpected trade message, there has to have been out-of-order or lost trades" );
                    spdlog::warn( "unexpected trade message, there has to have been out-of-order or lost trades, do nothing" );
                }
            } else {
                /* *******************************************
                 * This is a trade-through order, apply the order first and then add an ask order of 2 * total traded volume on the best bid level
                 * *******************************************/
                const auto total_trd_vol = trade.getTotalVol();
                newOrder( fmt::format("N {} 1 {} {}", 
                                      IdGenNeg::getInstance().genId(),
                                      total_trd_vol, 
                                      trade.price[0] ) );

                newOrder( fmt::format("N {} 1 {} {}", 
                                      IdGenNeg::getInstance().genId(),
                                      2 * total_trd_vol, 
                                      best_bid_px ) );
            }
        } else {
            // Seller is the liquidity taker
            const auto best_ask_px = getBestAsk().price;
            const int lvl_cnt = trade.getLvlCnt();

            if (lvl_cnt == 1) {
                const auto trd_px = trade.price[0];
                
                if( trd_px < best_ask_px ) {
                    static std::unordered_map<double, int> recored_trds_vol;
                    static double last_unconsumed_lvl = 0;

                    if ( trd_px == last_unconsumed_lvl ) {
                        recored_trds_vol[trd_px] += trade.getTotalVol();
                    } else {
                        newOrder( Order( fmt::format("N {} 0 {} {}", 
                                                     IdGenNeg::getInstance().genId(),
                                                     2 * recored_trds_vol[trd_px],
                                                     last_unconsumed_lvl ) ) );
                        recored_trds_vol.erase( last_unconsumed_lvl );
                        last_unconsumed_lvl = trd_px;
                    }
                } else if ( trd_px == best_ask_px ) {
                    const auto trd_qty = trade.getTotalVol();
                    if ( trd_qty < getBestBid().quantity ) {
                        // simply reflect the trade information on the book
                        newOrder( Order( fmt::format("N {} 0 {} {}", 
                                                     IdGenNeg::getInstance().genId(),
                                                     trd_qty,
                                                     best_ask_px ) ) );
                    } else {
                        // trade quantity + not yet received aggresive orders
                        newOrder( Order( fmt::format("N {} 0 {} {}", 
                                                     IdGenNeg::getInstance().genId(),
                                                     3 * trd_qty,
                                                     best_ask_px ) ) );
                    }
                    
                } else {
                    spdlog::warn( "unexpected trade message, there has to have been out-of-order or lost trades, do nothing" );
                }
            } else {
                /* *******************************************
                 * This is a trade-through order, apply the order first and then add an ask order of 2 * total traded volume on the best bid level
                 * *******************************************/
                const auto total_trd_vol = trade.getTotalVol();
                newOrder( fmt::format("N {} 0 {} {}", 
                                      IdGenNeg::getInstance().genId(),
                                      total_trd_vol, 
                                      trade.price[trade.getLvlCnt()-1] ) );

                newOrder( fmt::format("N {} 0 {} {}", 
                                      IdGenNeg::getInstance().genId(),
                                      2 * total_trd_vol, 
                                      best_ask_px ) );
            }
        }
    }


    L3Book( std::vector<Order>& orders )
    {
        for(auto& order: orders)
        {
            // newOrder( order );
            applyOrder( order );
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

        if ( askBook.empty() )
            ss << "<Empty>\n";
        for( auto rit = askBook.rbegin(); rit != askBook.rend(); ++rit ) {
            ss << rit->second.toString() << '\n';
        }

        ss << "--^ ASK SIDE--------BID SIDE V---\n";

        if ( bidBook.empty() )
            ss << "<Empty>\n";
        for( auto it = bidBook.begin(); it != bidBook.end(); ++it ) {
            ss << it->second.toString() << '\n';
        }

        return ss.str();
    }

    /**
     *  @brief  aggregate all orders in L3Book to producd an L2Book
     */
    L2Book agg() const
    {
        OneSideBook<L2PriceLevel, BidComparator> bidL2Book;
        OneSideBook<L2PriceLevel, AskComparator> askL2Book;

        for(const auto& [px, level]: bidBook) {
            bidL2Book[px] = level.toL2PriceLevel();
        }
        for(const auto& [px, level]: askBook) {
            askL2Book[px] = level.toL2PriceLevel();
        }

        return { bidL2Book, askL2Book, bidSideSize, askSideSize };
    }
}; // class L3Book


} // namespace sob


#endif
