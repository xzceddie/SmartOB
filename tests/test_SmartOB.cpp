#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <Order.h>
#include <OrderBook.h>
#include <L3OrderBook.h>
#include <SmartOrderBook.h>
#include <vector>

/**
 *  sync means the streams are in the order of tuple ( order, snapshot, [trade] )
 *      the trade is possible because there might not be a trade
 *      order can be either a Normal order, Cancel order or a Reprice order
 */

TEST_CASE( "test_SmartOrderBook_sync", "1" )
{
    spdlog::set_level(spdlog::level::debug);
    std::string o1("N 0 1 100 1.5");
    std::string s1{"S 0 1 1.5 100"};

    std::string o2("N 1 1 100 1.4");
    std::string s2{"S 0 2 1.4 100 1.5 100"};

    std::string o3("N 2 1 200 1.4");
    std::string s3{"S 0 2 1.4 300 1.5 100"};

    std::string o4("N 3 0 50 1.3");
    std::string s4{"S 1 2 1.3 50 1.4 300 1.5 100"};

    std::string o5("N 4 0 100 1.3");
    std::string s5{"S 1 2 1.3 150 1.4 300 1.5 100"};

    std::string o6("N 5 0 100 1.2");
    std::string s6{"S 2 2 1.2 100 1.3 150 1.4 300 1.5 100"};

    sob::SmartOrderBook SmartOB;

    SmartOB.applyMessage( o1 );
    SmartOB.applyMessage( s1 );
    SmartOB.applyMessage( o2 );
    SmartOB.applyMessage( s2 );
    SmartOB.applyMessage( o3 );
    SmartOB.applyMessage( s3 );
    SmartOB.applyMessage( o4 );
    SmartOB.applyMessage( s4 );
    SmartOB.applyMessage( o5 );
    SmartOB.applyMessage( s5 );
    SmartOB.applyMessage( o6 );
    SmartOB.applyMessage( s6 );

    spdlog::debug("[test_SmartOrderBook, 1] Constructed SmartOrderBook: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6 };
    sob::L3Book ob{ orders };

    REQUIRE( SmartOB.getLeaderBook()->toString() == ob.toString() );

    spdlog::set_level(spdlog::level::info);
}

TEST_CASE( "test_SmartOrderBook_sync_2", "1" )
{
    spdlog::set_level(spdlog::level::debug);
    std::string o1("N 0 1 100 1.5");
    std::string s1{"S 0 1 1.5 100"};

    std::string o2("N 1 1 100 1.4");
    std::string s2{"S 0 2 1.4 100 1.5 100"};

    std::string o3("N 2 1 200 1.4");
    std::string s3{"S 0 2 1.4 300 1.5 100"};

    std::string o4("N 3 0 50 1.3");
    std::string s4{"S 1 2 1.3 50 1.4 300 1.5 100"};

    std::string o5("N 4 0 100 1.3");
    std::string s5{"S 1 2 1.3 150 1.4 300 1.5 100"};

    std::string o6("N 5 0 100 1.2");
    std::string s6{"S 2 2 1.2 100 1.3 150 1.4 300 1.5 100"};

    std::string o7{ "N 6 0 50 1.4" };
    std::string s7{"S 2 2 1.2 100 1.3 150 1.4 250 1.5 100"};
    std::string t0{"T 0 1.4 50"};           // This is the first trade order that happens

    sob::SmartOrderBook SmartOB;

    SmartOB.applyMessage( o1 );
    SmartOB.applyMessage( s1 );
    SmartOB.applyMessage( o2 );
    SmartOB.applyMessage( s2 );
    SmartOB.applyMessage( o3 );
    SmartOB.applyMessage( s3 );
    SmartOB.applyMessage( o4 );
    SmartOB.applyMessage( s4 );
    SmartOB.applyMessage( o5 );
    SmartOB.applyMessage( s5 );
    SmartOB.applyMessage( o6 );
    SmartOB.applyMessage( s6 );

    SmartOB.applyMessage( o7 );
    SmartOB.applyMessage( s7 );
    SmartOB.applyMessage( t0 );

    spdlog::debug("[test_SmartOrderBook, 1] Constructed SmartOrderBook: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6, o7 };
    sob::L3Book ob{ orders };

    REQUIRE( SmartOB.getLeaderBook()->toString() == ob.toString() );

    spdlog::set_level(spdlog::level::info);
}
