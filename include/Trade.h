#ifndef TRADE_H
#define TRADE_H

#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>
#include <sstream>

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

    bool operator==( const Trade& rhs ) const
    {
        return price == rhs.price
            && volume == rhs.volume
            && isSell == rhs.isSell;
    }

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

    /**
     *  example msg:
     *      T 1(isSell) 0.9(first px) 10(first qty) 1.0 5 1.1 30
     */
    Trade( const std::string& msg )
    {
        if ( msg[0] != 'T' ) {
            throw std::runtime_error( "First char for a trade message must be T" );
        }

        std::stringstream ss{ msg };

        std::string _;
        ss >> _;
        
        ss >> isSell;

        while( !ss.eof() ) {
            double px;
            int vol;
            ss >> px >> vol;
            price.push_back( px );
            volume.push_back( vol );
        }
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "T " << (isSell ? "1" : "0") << " ";
        if ( price.size() == 0 ) {
            return ss.str();
        }

        for( int i = 0; i < price.size() - 1; i++ ) {
            ss << price[i] << " " << volume[i] << " ";
        }
        ss << price[price.size() - 1] << " " << volume[volume.size() - 1];
        return ss.str();
    }

}; // sturct Trade

} // namespace sob




#endif
