#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <Order.h>
#include <OrderBook.h>
#include <L3OrderBook.h>
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


TEST_CASE("test_L3OrderBook", "1")
{
    spdlog::set_level(spdlog::level::debug);
    sob::Order o1("N 0 1 100 1.5");
    sob::Order o2("N 1 1 100 1.4");
    sob::Order o3("N 2 1 200 1.4");
    sob::Order o4("N 3 0 50 1.3");
    sob::Order o5("N 4 0 100 1.3");
    sob::Order o6("N 5 0 100 1.2");

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6 };

    sob::L3Book ob{ orders };

    spdlog::debug("[test_L3OrderBook, 1] Constructed OrderBook: ");
    std::cout << ob.toString() << std::endl;

    REQUIRE( ob.getBidSideSize() == 250 );
    REQUIRE( ob.getAskSideSize() == 400 );

    REQUIRE( ob.getBidSideDepth() == 2 );
    REQUIRE( ob.getAskSideDepth() == 2 );

    REQUIRE( ob.getBidSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.3, 150 ) );
    REQUIRE( ob.getAskSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.4, 300 ) );

    spdlog::set_level( spdlog::level::info );
}

TEST_CASE("test_L3OrderBook_match_buy", "1")
{
    spdlog::set_level(spdlog::level::debug);
    sob::Order o1("N 0 1 100 1.5");
    sob::Order o2("N 1 1 100 1.4");
    sob::Order o3("N 2 1 200 1.4");
    sob::Order o4("N 3 0 50 1.3");
    sob::Order o5("N 4 0 100 1.3");
    sob::Order o6("N 5 0 100 1.2");

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6 };

    sob::L3Book ob{ orders };

    spdlog::debug("[test_L3OrderBook, 1] Constructed OrderBook: ");

    // aggressive orders
    {
        sob::Order o7{ "N 6 0 50 1.4" };
        ob.newOrder( o7 );
        REQUIRE( ob.getAskSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.4, 250 ) );
        REQUIRE( ( !ob.queryOrderId( 6 ) ) ); // because the order has been matched so it will not sit in the order book any more
        //
        sob::Order o8{ "N 7 0 50 1.4" };
        ob.newOrder( o8 );
        REQUIRE( ob.getAskSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.4, 200 ) );
        REQUIRE( ob.getAskSide().begin()->second.orders.size() == 1 ); // because the best ask level's best order has been filled
        //
        sob::Order o9{ "N 8 0 200 1.5" };
        ob.newOrder( o9 );
        REQUIRE( ob.getAskSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.5, 100 ) ); // the best ask level has been devoured
        REQUIRE( ob.getAskSideDepth() == 1 );
        REQUIRE( ob.getAskSideSize() == 100 );

        sob::Order o10{ "N 9 1 50 1.35" };  // NOTE: this is a sell order
        ob.newOrder( o10 );
        REQUIRE( ob.getAskSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.35, 50 ) );
        REQUIRE( ob.getAskSideDepth() == 2 );
        REQUIRE( ob.getAskSideSize() == 150 );

        sob::Order o11{ "N 10 0 200 1.5" };
        ob.newOrder( o11 );
        REQUIRE( ob.getAskSideDepth() == 0 );
        REQUIRE( ob.getBidSideDepth() == 3 );
        REQUIRE( ob.getBidSideSize() == 300 );
    }
    std::cout << ob.toString() << std::endl;
    spdlog::set_level( spdlog::level::info );
}

TEST_CASE("test_L3OrderBook_match_sell", "1")
{
    spdlog::set_level(spdlog::level::debug);
    sob::Order o1("N 0 1 100 1.5");
    sob::Order o2("N 1 1 100 1.4");
    sob::Order o3("N 2 1 200 1.4");
    sob::Order o4("N 3 0 50 1.3");
    sob::Order o5("N 4 0 100 1.3");
    sob::Order o6("N 5 0 100 1.2");

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6 };
    sob::L3Book ob{ orders };

    spdlog::debug("[test_L3OrderBook, 1] Constructed OrderBook: ");

    // aggressive orders
    {
        sob::Order o7{ "N 6 1 20 1.3" };
        ob.newOrder( o7 );
        REQUIRE( ob.getBidSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.3, 130 ) );
        REQUIRE( ( !ob.queryOrderId( 6 ) ) ); // because the order has been matched so it will not sit in the order book any more
        
        sob::Order o8{ "N 7 1 30 1.3" };
        ob.newOrder( o8 );
        REQUIRE( ob.getBidSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.3, 100 ) );
        REQUIRE( ob.getBidSide().begin()->second.orders.size() == 1 ); // because the best ask level's best order has been filled
        //
        sob::Order o9{ "N 8 1 100 1.2" };
        ob.newOrder( o9 );
        REQUIRE( ob.getBidSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.2, 100 ) ); // the best ask level has been devoured
        REQUIRE( ob.getBidSideDepth() == 1 );
        REQUIRE( ob.getBidSideSize() == 100 );
        //
        sob::Order o10{ "N 9 0 50 1.25" };  // NOTE: this is a sell order
        ob.newOrder( o10 );
        REQUIRE( ob.getBidSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.25, 50 ) );
        REQUIRE( ob.getBidSideDepth() == 2 );
        REQUIRE( ob.getBidSideSize() == 150 );
        //
        sob::Order o11{ "N 10 1 200 1.2" };
        ob.newOrder( o11 );
        REQUIRE( ob.getBidSideDepth() == 0 );
        REQUIRE( ob.getAskSideDepth() == 3 );
        REQUIRE( ob.getAskSideSize() == 450 );

        REQUIRE( ( *ob.queryOrderId( 10 ) )->size == 50 );
        REQUIRE( ( *ob.queryOrderId( 10 ) )->price == 1.2 );
        REQUIRE( ( *ob.queryOrderId( 1 ) )->size == 100 );
        REQUIRE( ( *ob.queryOrderId( 1 ) )->price == 1.4 );
    }
    std::cout << ob.toString() << std::endl;
    spdlog::set_level( spdlog::level::info );
}


TEST_CASE("test_L3OrderBook_cancel", "1")
{
    spdlog::set_level(spdlog::level::debug);
    sob::Order o1("N 0 1 100 1.5");
    sob::Order o2("N 1 1 100 1.4");
    sob::Order o3("N 2 1 200 1.4");
    sob::Order o4("N 3 0 50 1.3");
    sob::Order o5("N 4 0 100 1.3");
    sob::Order o6("N 5 0 100 1.2");

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6 };
    sob::L3Book ob{ orders };

    spdlog::debug("[test_L3OrderBook, 1] Constructed OrderBook: ");

    {
        {
            sob::Order o7{ "C 6 0 0 0 1 0 0" };
            const bool canceled = ob.cancelOrder( o7 );
            REQUIRE( canceled );
            REQUIRE(  ob.getAskSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.4, 200 ) );
        }

        {
            sob::Order o8{ "C 7 0 0 0 4 0 0" };
            const bool canceled = ob.cancelOrder( o8 );
            REQUIRE( canceled );
            REQUIRE(  ob.getBidSide().begin()->second.toL2PriceLevel() == sob::L2PriceLevel( 1.3, 50 ) );
        }
    }
    std::cout << ob.toString() << std::endl;
    spdlog::set_level( spdlog::level::info );
}
