#ifndef ORDER_H
#define ORDER_H

#include <iostream>
// #include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <list>
#include <map>
#include <unordered_map>
#include <sstream>
#include <optional>
#include <algorithm>
#include <string_view>
#include <dBuffer.h>


namespace sob {


struct L2PriceLevel
{
    double price;
    int quantity;

    friend std::ostream& operator<<(std::ostream& os, const L2PriceLevel& l)
    {
        os << "L2PriceLevel{price=" << l.price
           << ", quantity=" << l.quantity
           << "}";
        return os;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    L2PriceLevel() = default;

    L2PriceLevel( const double px, const int qty )
    : price{px}, quantity{qty}
    {}

    // fmt: px, qty
    L2PriceLevel( const std::string& str )
    {
        std::stringstream ss(str);
        try {
            ss >> price >> quantity;
        } catch ( const std::exception& e ) {
            spdlog::error("Got error when reading L2PriceLevel from string: {}, the exception: {}"
                          , str, e.what());
            throw;
        }
    }

    bool operator==( const L2PriceLevel& rhs ) const
    {
        return ( price == rhs.price && quantity == rhs.quantity );
    }
}; // struct L2PriceLevel


struct L2PxLvlPair
{
    std::optional<L2PriceLevel> bid;
    std::optional<L2PriceLevel> ask;

    friend std::ostream& operator<<(std::ostream& os, const L2PxLvlPair& p)
    {
        
        if (p.bid) {
            os << p.bid.value().quantity << '@' << p.bid.value().price;
        } else {
            os << "<empty>";
        }
        os << " X ";

        if (p.ask) {
            os << p.ask.value().price << '@' << p.ask.value().quantity;
        } else {
            os << "<empty>";
        }
        return os;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }
}; // sturct L2PxLvlPair


struct OrderInfo
{
    // what should I put in here?
}; // struct OrderInfo

enum class OrderType
{
    Nornal,
    Cancel,
    Reprice
}; // enum class OrderType


struct Order
{
    int orderId;
    bool isSell;
    int size;
    double price;

    bool bIsCancel{false}; // for cancel order
    bool bIsReprice{false}; // for cancel order
    std::optional<int> oldId; // for cancel and reprice orders
    std::optional<double> oldPx; // for reprice order, if this is not null, then it is a reprice order
    std::optional<int> oldSz; // for reprice order, if this is not null, then it is a reprice order

    std::shared_ptr<OrderInfo> info;

    OrderType getType() const
    {
        if( bIsCancel ) {
            return OrderType::Cancel;
        } else if ( bIsReprice ) {
            return OrderType::Reprice;
        } else {
            return OrderType::Nornal;
        }
    }

    bool isCancel() const
    {
        return bIsCancel;
    }

    bool isReprice() const
    {
        // return oldPx == std::optional<double>{};
        return bIsReprice;
    }

    friend std::ostream& operator<<(std::ostream& os, const Order& o)
    {
        const auto get_type_str = [](const Order& o) -> std::string_view
        {
            if (o.isCancel()) {
                return "C";
            } else if ( o.isReprice() ) {
                return "R";
            } else {
                return "N";
            }
        };
        const auto get_side_str = [](const Order& o) -> std::string_view
        {
            if (o.isSell) {
                return "sell";
            } else {
                return "buy";
            }
        };
        os << "Order{" << get_type_str(o)
           << ", id=" << o.orderId
           << ", " << get_side_str(o)
           << ", sz=" << o.size
           << ", px=" << o.price
           // << ", bIsCancel=" << o.bIsCancel
        ;
        if(o.isReprice() || o.isCancel()) {
            os << ", oldId=" << *o.oldId;
        }
        if(o.isReprice()) {
            os << ", oldPx=" << *o.oldPx
               << ", oldSz=" << *o.oldSz;
        }
        os << "}";
        return os;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    // fmt: type{Normal{'N'}, cancel{'C'}, reprice{'R'}}, orderId, isSell {0, 1}, size, price, [oldId], [oldPx], [oldSz]
    Order( const std::string& str )
    {
        std::stringstream ss(str);
        try {
            std::string type; 
            ss >> type;
            if (type == "N") {
                ss >> orderId >> isSell >> size >> price; 
            } else if (type == "C") {
                ss >> orderId ;
                bIsCancel = true;
                int _{};
                ss >> _ >> _ >> _ >> *oldId >> _ >> _;
                // std::cout << "Got oldId: " << *oldId << std::endl;
            } else if (type == "R") {
                ss >> orderId >> isSell >> size >> price >> *oldId >> *oldPx >> *oldSz;
                // std::cout << "old ID: " << *oldId << std::endl;
                // std::cout << "old Px: " << *oldPx << std::endl;
                // std::cout << "old Sz: " << *oldSz << std::endl;
                bIsReprice = true;
            }
        } catch ( const std::exception& e ) {
            spdlog::error("Got error when reading L2PriceLevel from string: {}, the exception: {}"
                          , str, e.what());
            throw;
        }
    }

}; // struct Order


template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
struct L3PriceLevel
{
    double price;
    int quantity;
    int numOrders;
    // std::list<Order> orders;
    // dBuffer<Order, boost::circular_buffer> orders;
    dBuffer<Order, BuffType> orders;

    friend std::ostream& operator<<(std::ostream& os, const L3PriceLevel& l)
    {
        os << "L3PxLvl{px=" << l.price
           << ", qty=" << l.quantity
           << ", #orders=" << l.numOrders
           << ", orders={";

        for (auto& o : l.orders) 
            os << o << ", ";
        os << "}";
        return os;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    auto toL2PriceLevel() const
    {
        return L2PriceLevel{ price, quantity };
    }

    auto addNewOrder( const Order& order )
    {
        assert( price == order.price );
        quantity += order.size;
        numOrders += 1;
        orders.push_back( order );
        return std::prev( orders.end() );
    }

    // order must not take all the liquidity
    // return how much liquidity is taken
    // size_t matchOrder( Order& order, std::unordered_map<int, std::list<Order>::iterator>& orderMap)
    size_t matchOrder( Order& order, std::unordered_map<int, typename BuffType<Order>::iterator>& orderMap)
    {
        size_t res{};
        while (true) {
            auto& front_order = orders.front();
            if( order.size > front_order.size ) {
                const auto popped_id = orders.front().orderId;
                order.size -= front_order.size;
                res += front_order.size;
                orders.pop_front();
                orderMap.erase( popped_id );

                numOrders -= 1;
                quantity -= front_order.size;
            } else {
                orders.front().size -= order.size;
                res += order.size;
                quantity -= order.size;
                if( orders.front().size == 0 ) {
                    numOrders -= 1;
                    orderMap.erase( orders.front().orderId );
                    orders.pop_front();
                }
                break;
            }
        }
        return res;
    }

    L3PriceLevel() = default;
    
    L3PriceLevel( const std::vector<Order>& orders_ )
    {
        if (orders_.empty()) {
            throw std::runtime_error( "[L3PriceLevel::L3PriceLevel] Got empty orders!" );
        } else {
            price = orders_[0].price;
            numOrders = orders_.size();
            std::copy( orders_.begin(), orders_.end(), std::back_inserter( orders ) );
            quantity = 0;
            for( const auto& o: orders_ ) {
                quantity += o.size;
            }
        }
    }

    // if quantity is known beforehand, it is more efficient to call this one;
    L3PriceLevel( const std::vector<Order>& orders_, const int quantity_ )
    {
        if (orders_.empty()) {
            throw std::runtime_error( "[L3PriceLevel::L3PriceLevel] Got empty orders!" );
        } else {
            price = orders_[0].price;
            numOrders = orders_.size();
            std::copy( orders_.begin(), orders_.end(), std::back_inserter( orders ) );
            quantity = quantity_;
        }
    }
}; // struct L3PriceLevel

template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
struct L3PxLvlPair
{
    std::optional<L3PriceLevel<BuffType>> bid;
    std::optional<L3PriceLevel<BuffType>> ask;

    friend std::ostream& operator<<(std::ostream& os, const L3PxLvlPair& p)
    {
        
        if (p.bid) {
            os << p.bid.value().quantity << '@' << p.bid.value().price;
        } else {
            os << "<empty>";
        }
        os << " X ";

        if (p.ask) {
            os << p.ask.value().price << '@' << p.ask.value().quantity;
        } else {
            os << "<empty>";
        }
        return os;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    auto toL2PxLvlPair() const
    {
        std::optional<L2PriceLevel> bid_, ask_;
        if ( bid ) {
            bid_ = bid->toL2PriceLevel();
        }
        if ( ask ) {
            ask_ = ask->toL2PriceLevel();
        }

        return L2PxLvlPair{ bid_, ask_ };
    }
}; // sturct L2PxLvlPair



} // namespace sob



#endif
