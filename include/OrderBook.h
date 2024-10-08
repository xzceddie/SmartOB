#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <Order.h>
#include <memory>



namespace sob {


struct BidComparator
{
    bool operator()( const double& lhs, const double& rhs ) const
    {
        if ( lhs > rhs ) {
            return true;
        } 
        return false;
    }
}; // struct bComparator
//
struct AskComparator
{
    bool operator()( const double& lhs, const double& rhs ) const
    {
        if ( lhs< rhs) {
            return true;
        } 
        return false;
    }
}; // struct bComparator



template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;

/**
 *  @brief  An Orderbook that keeps the aggregated information on all levels
 *  @NOTE   L2Book does not support cancelation or repricing / modifying, because it does not do the book-keeping of the order information
 *              Thus it does not know the status of the to-be-canceled or to-be-modified order
 *  @NOTE   This is not a Template class;
 */
class L2Book
{
private:
    OneSideBook<L2PriceLevel, BidComparator> bidBook{ BidComparator{} };
    OneSideBook<L2PriceLevel, AskComparator> askBook{ AskComparator{} };

    size_t bidSideSize = 0;
    size_t askSideSize = 0;

public:
    bool operator==( const L2Book& rhs )
    {
        return bidBook == rhs.bidBook 
            && askBook == rhs.askBook
            && bidSideSize == rhs.bidSideSize
            && askSideSize == rhs.askSideSize
        ;
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

    OneSideBook<L2PriceLevel, BidComparator>& getBidSide()
    {
        return bidBook;
    }

    OneSideBook<L2PriceLevel, AskComparator>& getAskSide()
    {
        return askBook;
    }

    L2PriceLevel getBestBid() const
    {
        if ( bidBook.empty() ) {
            return L2PriceLevel{};
        }
        return bidBook.begin()->second;
    }

    L2PriceLevel getBestAsk() const
    {
        if ( askBook.empty() ) {
            return L2PriceLevel{};
        }
        return askBook.begin()->second;
    }

public:
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

    auto getBestMarket() const
    {
        L2PxLvlPair ret;
        if ( !askBook.empty() ) {
            ret.ask = askBook.begin()->second;
        }
        if ( !bidBook.empty() ) {
            ret.bid = bidBook.begin()->second;
        }
        return ret;
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

    // @return  true if took liquidity, false if added liquidity
    bool newOrder( Order& order )
    {
        // assert( !order.isCancel() );
        // assert( !order.isReprice() );
        if( order.isCancel() ) {
            throw std::runtime_error( "[OrderBook::newOrder]: cannot handle cancel order" );
        }
        if( order.isReprice() ) {
            throw std::runtime_error( "[OrderBook::newOrder]: cannot handle reprice order" );
        }

        if( order.isSell ) {
            if ( bidBook.empty() || order.price > getBestBid().price ) {
                // Not aggressive/cross/market Order, quote orders only
                askSideSize += order.size;
                if ( askBook.find( order.price ) == askBook.end() ) {
                    askBook[order.price] = L2PriceLevel{order.price, order.size};
                } else {
                    askBook[order.price].quantity += order.size;
                }
                return false;
            } else {
                // aggressive order
                for( auto it = bidBook.begin(); it != bidBook.end(); ) {
                    const auto best_bid_size = getBestBid().quantity;
                    if (it -> first < order.price ) {
                        askBook[order.price] = L2PriceLevel{order.price, order.size};
                        askSideSize += order.size;
                        return true;
                    }

                    if( best_bid_size <= order.size ) {
                        order.size -= best_bid_size;
                        it = bidBook.erase( it );
                        bidSideSize -= best_bid_size;
                    } else {
                        bidSideSize -= (it -> second).matchOrder( order );
                        return true;
                    }

                }

                if( order.size != 0 ) {
                    // All bid levels has been consumed, added a new level in the ask book
                    askBook[order.price] = L2PriceLevel{order.price, order.size};
                    askSideSize += order.size;
                }
            }
            return true;
        } else {
            // Not aggressive/cross/market Order, quote orders only
            if ( askBook.empty() || order.price < getBestAsk().price ) {
                bidSideSize += order.size;
                if ( bidBook.find( order.price ) == bidBook.end() ) {
                    bidBook[order.price] = L2PriceLevel{order.price, order.size};
                } else {
                    bidBook[order.price].quantity += order.size;
                }
                return false;
            } else {
                // aggressive order
                for( auto it = askBook.begin(); it != askBook.end(); ) {
                    const auto best_ask_size = getBestAsk().quantity;
                    if (it -> first > order.price ) {
                        bidBook[order.price] = L2PriceLevel{order.price, order.size};
                        bidSideSize += order.size;
                        return true;
                    }

                    if( best_ask_size <= order.size ) {
                        order.size -= best_ask_size;
                        it = askBook.erase( it );
                        askSideSize -= best_ask_size;
                    } else {
                        askSideSize -= (it -> second).matchOrder( order );
                        return true;
                    }
                }

                if( order.size != 0 ) {
                    // All ask levels has been consumed, added a new level in the bid book
                    bidBook[order.price] = L2PriceLevel{order.price, order.size};
                    bidSideSize += order.size;
                }
            }
            return true;
        }

    }

    // bool modifyOrder( const Order& order )
    // {
    //     return false;
    // }

    // bool cancelOrder( const Order& order )
    // {
    //     return false;
    // }

    L2Book() = default;

    L2Book( std::vector<Order>& orders )
    {
        for(auto& order: orders)
        {
            newOrder( order );
        }
    }

    L2Book( OneSideBook<L2PriceLevel, BidComparator>& bids, OneSideBook<L2PriceLevel, AskComparator>& asks )
    : bidBook{ bids }
    , askBook{ asks }
    {}

    L2Book( OneSideBook<L2PriceLevel, BidComparator>& bids,
            OneSideBook<L2PriceLevel, AskComparator>& asks,
            const size_t bidSideSize,
            const size_t askSideSize )
    : bidBook{ bids }
    , askBook{ asks }
    , bidSideSize{ bidSideSize }
    , askSideSize{ askSideSize }
    {}

    /**
     * example msg: 
     *  "S 3(bid book depth) 3(ask book depth) 1.0(px) 5(sz) 1.1(px) 2(sz) 1.2 10 1.3 20 1.4 5 1.5 6"
     */
    L2Book( const std::string& msg )
    {
        if ( msg[0] != 'S' ) {
            throw std::runtime_error( "First char for a snapshot message must be S" );
        }
        std::stringstream ss(msg);

        std::string tmp;
        int bid_depth, ask_depth;
        ss >> tmp >> bid_depth >> ask_depth;


        for ( int i=0; i < bid_depth; i++ ) {
            double px;
            int sz;
            ss >> px >> sz;
            bidBook[px] = L2PriceLevel{px, sz};
        }

        for ( int i=0; i < ask_depth; i++ ) {
            double px;
            int sz;
            ss >> px >> sz;
            askBook[px] = L2PriceLevel{px, sz};
        }
    }

    std::string to_simple_string() const
    {
        std::stringstream ss;
        ss << "S " << bidBook.size() << " " << askBook.size() << " ";
        for ( const auto& [px, level] : bidBook ) {
            ss << px << " " << level.quantity << " ";
        }
        for ( const auto& [px, level] : askBook ) {
            ss << px << " " << level.quantity << " ";
        }
        return ss.str().substr(0, ss.str().size() - 1); // because we do not need the last ' '
    }
}; // class L2Book

} // namespace sob

#endif
