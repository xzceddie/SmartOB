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

        for( auto rit = askBook.rbegin(); rit != askBook.rend(); ++rit ) {
            ss << rit->second.toString() << '\n';
        }

        ss << "------\n";

        for( auto it = bidBook.begin(); it != bidBook.end(); ++it ) {
            ss << it->second.toString() << '\n';
        }

        return ss.str();
    }

    BestMarket getBestMarket() const
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
    virtual bool newOrder( const Order& order )
    {
        assert( !order.isCancel() );
        assert( !order.isReprice() );
        if( order.isSell ) {
            // Not aggressive/cross/market Order, quote orders only
            if ( order.price > getBestBid().price ) {
                askSideSize += order.size;
                if ( askBook.find( order.price ) == askBook.end() ) {
                    askBook[order.price] = L2PriceLevel{order.price, order.size};
                } else {
                    askBook[order.price].quantity += order.size;
                }
                return false;
            }
            return true;
        } else {
            // Not aggressive/cross/market Order, quote orders only
            if ( order.price < getBestAsk().price ) {
                bidSideSize += order.size;
                if ( bidBook.find( order.price ) == bidBook.end() ) {
                    bidBook[order.price] = L2PriceLevel{order.price, order.size};
                } else {
                    bidBook[order.price].quantity += order.size;
                }
                return false;
            }
            return true;
        }

    }

    // TODO: Implement modifyOrder
    virtual void modifyOrder( const Order& order )
    {
    }

    // TODO: Implement cancelOrder
    virtual void cancelOrder( const Order& order )
    {
    }

    L2Book() = default;

    L2Book( const std::vector<Order>& orders )
    {
        for(const auto& order: orders)
        {
            newOrder( order );
        }
    }

}; // class L2Book

} // namespace sob

#endif
