#ifndef SMART_ORDER_BOOK_H
#define SMART_ORDER_BOOK_H


#include <L3OrderBook.h>



namespace sob {


template<typename LevelType, typename Comparator>
using OneSideBook = std::map<double, LevelType, Comparator>;


class Synchronizer
{
}; // class Synchronizer


/**
 *  @brief This is used to gather the latest information received and produce a most up-to-date order book
 *  @NOTE  The result contain some guesses/deductions,
 *  @NOTE  This is not a Template class;
 */
template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
class SmartOrderBook
{
private:
    L3Book<BuffType> bookGroundTruth;
    L3Book<BuffType> bookTrade;
    L3Book<BuffType> bookSnapShot;

    Synchronizer synchronizer;
}; // class SmartOrderBook




} // namespace sob





#endif
