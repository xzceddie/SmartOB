#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <Order.h>
#include <OrderBook.h>
#include <vector>


TEST_CASE("test_L2OrderBook", "1")
{
    spdlog::set_level(spdlog::level::debug);
    sob::Order o1("N 0 1 100 1.5");
    sob::Order o2("N 1 1 100 1.4");
    sob::Order o3("N 2 1 200 1.4");
    sob::Order o4("N 3 0 50 1.3");
    sob::Order o5("N 4 0 100 1.3");
    sob::Order o6("N 5 0 100 1.2");

    sob::L2Book ob{ std::vector<sob::Order>{o1, o2, o3, o4, o5, o6} };

    spdlog::debug("[test_L2OrderBook, 1] Constructed OrderBook: ");
    std::cout << ob.toString() << std::endl;

    REQUIRE( ob.getBidSideSize() == 250 );
    REQUIRE( ob.getAskSideSize() == 400 );

    REQUIRE( ob.getBidSideDepth() == 2 );
    REQUIRE( ob.getAskSideDepth() == 2 );

    REQUIRE( ob.getBidSide().begin()->second == sob::L2PriceLevel( 1.3, 150 ) );
    REQUIRE( ob.getAskSide().begin()->second == sob::L2PriceLevel( 1.4, 300 ) );

    spdlog::set_level( spdlog::level::info );
}





