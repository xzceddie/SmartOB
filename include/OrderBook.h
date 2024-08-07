#include <include/Order.h>
#include <memory>



namespace sob {


struct BidComparator
{
    bool operator()( const L2PriceLevel& lhs, const L2PriceLevel& rhs ) const
    {
        if ( lhs.price > rhs.price ) {
            return true;
        } 
        return false;
    }
}; // struct bComparator
//
struct AskComparator
{
    bool operator()( const L2PriceLevel& lhs, const L2PriceLevel& rhs ) const
    {
        if ( lhs.price < rhs.price ) {
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
    OneSideBook<L2PriceLevel, BidComparator> bidBook;
    OneSideBook<L2PriceLevel, AskComparator> askBook;

public:
    size_t getBidSideSize() const
    {
    }

    size_t getAskSideSize() const
    {
    }

    OneSideBook<L2PriceLevel, BidComparator>& getBidSide()
    {
        return bidBook;
    }

    OneSideBook<L2PriceLevel, AskComparator>& getAskSide()
    {
        return askBook;
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


    newOrder( const Order& order )
    {
    }

    modifyOrder( const Order& oldOrder, const Order& newOrder )
    {
    }

    cancelOrder( const Order& order )
    {
    }

    L2OrderBook() = default;

    L2OrderBook( std::vector<Order>& orders )
    {
        for(const auto& orders)
        {
            newOrder( orders );
        }
    }

}; // class L2Book

} // namespace sob
