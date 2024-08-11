#ifndef L3_ORDER_BOOK_LISTENER_H
#define L3_ORDER_BOOK_LISTENER_H

#include <L3OrderBook.h>
#include <memory>
#include <boost/circular_buffer.hpp>


namespace sob {

template <template <typename T, typename AllocT=std::allocator<T> > class BuffType>
class L3OrderBookListener
{
public:
    L3OrderBookListener()
    {}

    void subscribe( std::shared_ptr<L3Book<BuffType>> book )
    {
        book -> accept( this );
    }

    L3OrderBookListener(std::shared_ptr<L3Book<BuffType>> book)
    {
        subscribe( book );
    }

    virtual void onBookUpdate()
    {};
    // void onBookUpdate( std::shared_ptr<L3Book<BuffType>>& ) {};
    // void onBookUpdate( std::shared_ptr<L3Book<BuffType>>&, const Order& order ) = 0;
    virtual void onBookUpdate( L3Book<BuffType>*, const Order& order ) = 0;
    virtual void onBookUpdate( L3Book<BuffType>& )
    {};
    virtual void onBookUpdate( const L3Book<BuffType>& )
    {};

    // void onTradeMsg( std::shared_ptr<L3Book<BuffType>>& book, const Trade& trade ) = 0;
    virtual void onTradeMsg( L3Book<BuffType>* book, const Trade& trade ) = 0;
    // void onSnapShotMsg( std::shared_ptr<L3Book<BuffType>>& book ) = 0;
    virtual void onSnapShotMsg( L3Book<BuffType>* book ) = 0;

}; // class L3OrderBookListener



} // namespace sob




#endif
