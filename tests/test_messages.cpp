#define CATCH_CONFIG_MAIN
#include <Trade.h>
#include <catch2/catch_all.hpp>
#include <common.h>


TEST_CASE("test_messages_trade")
{
    spdlog::set_level( spdlog::level::debug );
    sob::Trade trade{ "T 1 0.9 10 1.1 5 1.2 30" };
    REQUIRE( trade.getLvlCnt() == 3 );
    REQUIRE( trade.price == std::vector<double>{ 0.9, 1.1, 1.2 } );
    REQUIRE( trade.volume == std::vector<int>{ 10, 5, 30 } );
    REQUIRE( trade.isSell == true );

    spdlog::debug( "[test_messages] Got trade: {}", trade.toString() );

    sob::Trade new_trade{ trade.toString() };
    REQUIRE( new_trade.getLvlCnt() == 3 );
    REQUIRE( new_trade.price == std::vector<double>{ 0.9, 1.1, 1.2 } );
    REQUIRE( new_trade.volume == std::vector<int>{ 10, 5, 30 } );
    REQUIRE( new_trade.isSell == true );

    REQUIRE( trade == new_trade );
    spdlog::set_level( spdlog::level::info );
}

TEST_CASE("test_messages_snapshot")
{
    REQUIRE( true );
}




