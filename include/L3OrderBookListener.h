#ifndef L3_ORDER_BOOK_LISTENER_H
#define L3_ORDER_BOOK_LISTENER_H

#include <L3OrderBook.h>
#include <memory>


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

    void onBookUpdate()
    {};
    void onBookUpdate( std::shared_ptr<L3Book<BuffType>>& ) {};
    void onBookUpdate( std::shared_ptr<L3Book<BuffType>>&, const Order& order ) = 0;
    void onBookUpdate( L3Book<BuffType>& )
    {};
    void onBookUpdate( const L3Book<BuffType>& )
    {};

}; // class L3OrderBookListener



} // namespace sob




#endif
