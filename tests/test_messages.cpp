#define CATCH_CONFIG_MAIN
#include <Trade.h>
#include <OrderBook.h>
#include <catch2/catch_all.hpp>
#include <common.h>


TEST_CASE("test_messages_trade", "1")
{
    spdlog::set_level( spdlog::level::debug );
    sob::Trade trade{ "T 1 0.9 10 1.1 5 1.2 30" };
    REQUIRE( trade.getLvlCnt() == 3 );
    REQUIRE( trade.price == std::vector<double>{ 0.9, 1.1, 1.2 } );
    REQUIRE( trade.volume == std::vector<int>{ 10, 5, 30 } );
    REQUIRE( trade.isSell == true );

    spdlog::debug( "[test_messages_trade] Got trade: {}", trade.toString() );

    sob::Trade new_trade{ trade.toString() };
    REQUIRE( new_trade.getLvlCnt() == 3 );
    REQUIRE( new_trade.price == std::vector<double>{ 0.9, 1.1, 1.2 } );
    REQUIRE( new_trade.volume == std::vector<int>{ 10, 5, 30 } );
    REQUIRE( new_trade.isSell == true );

    REQUIRE( trade == new_trade );
    spdlog::set_level( spdlog::level::info );
}

TEST_CASE("test_messages_snapshot", "1")
{
    spdlog::set_level( spdlog::level::debug );
    sob::L2Book ob{"S 3 3 1.0 5 1.1 2 1.2 10 1.3 20 1.4 5 1.5 6"};
    spdlog::debug( "[test_messages_snapshot] Got snapshot: \n{}", ob.toString() );

    REQUIRE( ob.getBidSide().begin()->second == sob::L2PriceLevel( 1.2, 10 ) );
    REQUIRE( ob.getAskSide().begin()->second == sob::L2PriceLevel( 1.3, 20 ) );
    REQUIRE( ob.getAskSideDepth() == 3 );
    REQUIRE( ob.getBidSideDepth() == 3 );

    spdlog::debug( "[test_messages_snapshot] Got snapshot simple: {}", ob.to_simple_string() );

    sob::L2Book new_ob{ ob.to_simple_string() };
    REQUIRE( ob == new_ob );
    spdlog::set_level( spdlog::level::info );
}

TEST_CASE("test_messages_snapshot_2", "1")
{
    spdlog::set_level( spdlog::level::debug );
    sob::L2Book ob{"S 2 3 1.2 100 1.3 150 1.35 50 1.4 50 1.3 250 1.5 100"};
    spdlog::debug( "[test_messages_snapshot] Got snapshot: \n{}", ob.toString() );

    spdlog::debug( "[test_messages_snapshot] Got snapshot simple: {}", ob.to_simple_string() );

    sob::L2Book new_ob{ ob.to_simple_string() };
    REQUIRE( ob == new_ob );
    spdlog::set_level( spdlog::level::info );
}



