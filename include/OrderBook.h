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

    // @return  true if took liquidity, false if added liquidity
    // TODO: implement match logics
    bool newOrder( Order& order )
    {
        assert( !order.isCancel() );
        assert( !order.isReprice() );
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
            }
            return true;
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

    L2Book() = default;

    L2Book( std::vector<Order>& orders )
    {
        for(auto& order: orders)
        {
            newOrder( order );
        }
    }

}; // class L2Book

} // namespace sob

#endif
