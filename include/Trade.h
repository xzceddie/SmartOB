#ifndef TRADE_H
#define TRADE_H

#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>

namespace sob {

/**
 *  @brief  Trade information
 *  @NOTE   It is guaranteed that the price are SORTED in ascending order, volume are in corresponding order
 *  @NOTE   One trade can cross multiple levels, thus multiple prices and volumes
 */
struct Trade
{
    std::vector<double> price;
    std::vector<int> volume;
    bool isSell;    // true: the seller is the liquidity taker, false: the buyer is the liquidity taker

    int getLvlCnt() const
    {
        return price.size();
    }

    int getTotalVol() const
    {
        return std::accumulate( volume.begin(), volume.end(), 0 );
    }

    double getAvgPrice() const
    {
        return std::accumulate( price.begin(), price.end(), 0.0 ) / getTotalVol();
    }

}; // sturct Trade

} // namespace sob




#endif
