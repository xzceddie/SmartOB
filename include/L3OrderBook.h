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

public:
    L3Book() = default;

    virtual bool newOrder( const Order& order )
    {
        assert( !order.isCancel() );
        assert( !order.isReprice() );

        if( order.isSell ) {
            // Not aggressive/cross/market Order, quote orders only
            if ( bidBook.empty() || order.price > getBestBid().price ) {
                askSideSize += order.size;
                if ( askBook.find( order.price ) == askBook.end() ) {
                    askBook[order.price] = L3PriceLevel{ std::vector<Order>{ order }};
                } else {
                    askBook[order.price].addNewOrder( order );
                }
                return false;
            }
            return true;
        } else {
            // Not aggressive/cross/market Order, quote orders only
            if ( askBook.empty() || order.price < getBestAsk().price ) {
                bidSideSize += order.size;
                if ( bidBook.find( order.price ) == bidBook.end() ) {
                    bidBook[order.price] = L3PriceLevel{ std::vector<Order>{ order }};
                } else {
                    bidBook[order.price].addNewOrder( order );
                }
                return false;
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

    L3Book( const std::vector<Order>& orders )
    {
        for(const auto& order: orders)
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
