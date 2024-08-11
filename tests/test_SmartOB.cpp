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

TEST_CASE( "test_SmartOrderBook_async", "1" )
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

    /** SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=300, #odr=2, odr={Odr{N, id=1, sell, sz=100, px=1.4}, Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    SmartOB.applyMessage( t0 );

    spdlog::debug("[test_SmartOrderBook, 1] SmartOrderBook when trade leads: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    SmartOB.applyMessage( o7 );
    SmartOB.applyMessage( s7 );

    spdlog::debug("[test_SmartOrderBook, 1] Constructed SmartOrderBook: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6, o7 };
    sob::L3Book ob{ orders };

    REQUIRE( SmartOB.getLeaderBook()->toString() == ob.toString() );

    spdlog::set_level(spdlog::level::info);
}

TEST_CASE( "test_SmartOrderBook_async_2", "1" )
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

    std::string o7{"N 6 0 50 1.4"};
    std::string s7{"S 2 2 1.2 100 1.3 150 1.4 250 1.5 100"};

    std::string o8{ "N 7 0 50 1.4" };
    std::string s8{ "S 2 2 1.2 100 1.3 150 1.4 200 1.5 100"};
    
    std::string t0{"T 0 1.4 50"};           // This is the first trade order that happens
    std::string t1{"T 0 1.4 50"};           // This is the first trade order that happens

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

    /** SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=300, #odr=2, odr={Odr{N, id=1, sell, sz=100, px=1.4}, Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    SmartOB.applyMessage( t0 );
    SmartOB.applyMessage( t1 );

    spdlog::debug("[test_SmartOrderBook, 1] SmartOrderBook when trade leads: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    SmartOB.applyMessage( o7 );
    SmartOB.applyMessage( s7 );
    SmartOB.applyMessage( o8 );
    SmartOB.applyMessage( s8 );

    
    /** Catched up == Order book SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=200, #odr=1, odr={Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    spdlog::debug("[test_SmartOrderBook, 1] Constructed SmartOrderBook: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6, o7, o8 };
    sob::L3Book ob{ orders };

    REQUIRE( SmartOB.getLeaderBook()->toString() == ob.toString() );

    spdlog::set_level(spdlog::level::info);
}

TEST_CASE( "test_SmartOrderBook_async_2(trade-through)", "1" )
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

    std::string o7{"N 6 0 50 1.4"};
    std::string s7{"S 2 2 1.2 100 1.3 150 1.4 250 1.5 100"};

    std::string o8{ "N 7 0 50 1.4" };
    std::string s8{ "S 2 2 1.2 100 1.3 150 1.4 200 1.5 100"};

    std::string o9{ "N 8 0 250 1.5" };
    std::string s9{ "S 2 1 1.2 100 1.3 150 1.5 50"};

    std::string o10{ "N 8 0 200 1.4" };             // fail to fill
    std::string s10{ "S 2 2 1.2 100 1.3 150 1.4 200 1.5 50"};

    std::string o11{ "N 8 0 300 1.4" };             // fail to fill
    std::string s11{ "S 2 2 1.2 100 1.3 150 1.4 500 1.5 50"};
    
    std::string t0{"T 0 1.4 50"};           // This is the first trade order that happens
    std::string t1{"T 0 1.4 50"};
    std::string t2{"T 0 1.4 200 1.5 50"};   // This is a trade-through

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

    /** SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=300, #odr=2, odr={Odr{N, id=1, sell, sz=100, px=1.4}, Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    SmartOB.applyMessage( t0 );
    SmartOB.applyMessage( t1 );
    SmartOB.applyMessage( t2 );

    /** SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=50, #odr=1, odr={Odr{N, id=0, sell, sz=50, px=1.5}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.4, qty=500, #odr=1, odr={Odr{N, id=-4, buy, sz=400, px=1.4}, }    [ id = -3 < 0 means this is a fake order ]
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    spdlog::debug("[test_SmartOrderBook, 1] SmartOrderBook when trade leads: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    SmartOB.applyMessage( o7 );
    SmartOB.applyMessage( s7 );
    SmartOB.applyMessage( o8 );
    SmartOB.applyMessage( s8 );
    SmartOB.applyMessage( o9 );
    SmartOB.applyMessage( s9 );
    SmartOB.applyMessage( o10 );
    SmartOB.applyMessage( s10 );
    SmartOB.applyMessage( o11 );
    SmartOB.applyMessage( s11 );

    
    /** Catched up == Order book SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=200, #odr=1, odr={Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    spdlog::debug("[test_SmartOrderBook, 1] Constructed SmartOrderBook: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6, o7, o8, o9, o10, o11 };
    sob::L3Book ob{ orders };

    REQUIRE( SmartOB.getLeaderBook()->toString() == ob.toString() );

    spdlog::set_level(spdlog::level::info);
}

TEST_CASE( "test_SmartOrderBook_async_2(new-level-within-spread)", "1" )
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

    std::string o7{"N 6 1 100 1.35"};                                // out-of-sync starting from here
    std::string s7{"S 2 3 1.2 100 1.3 150 1.35 100 1.4 100 1.5 100"};

    std::string o8{"N 7 0 50 1.35"};
    std::string s8{"S 2 3 1.2 100 1.3 150 1.35 50 1.4 50 1.3 250 1.5 100"};

    std::string o9{"N 8 0 50 1.35"};
    std::string s9{"S 2 2 1.2 100 1.3 150 1.4 50 1.3 250 1.5 100"};

    std::string o10{"N 9 0 10 1.4"};
    std::string s10{"S 3 2 1.2 100 1.3 150 1.35 200 1.4 40 1.3 250 1.5 100"};

    std::string o11{"N 10 0 200 1.35"};
    std::string s11{"S 3 2 1.2 100 1.3 150 1.35 200 1.4 40 1.3 250 1.5 100"};
    

    std::string t0{"T 0 1.35 50"};           // This is the first trade order that happens
    std::string t1{"T 0 1.35 50"};
    std::string t2{"T 0 1.4 10"};

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

    /** SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=300, #odr=2, odr={Odr{N, id=1, sell, sz=100, px=1.4}, Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    SmartOB.applyMessage( t0 );
    SmartOB.applyMessage( t1 );
    SmartOB.applyMessage( t2 );

    /** SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=50, #odr=1, odr={Odr{N, id=0, sell, sz=50, px=1.5}, }
     * L3Lvl{px=1.4, qty=290, #odr=2, odr={Odr{N, id=1, sell, sz=90, px=1.4}, Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.35, qty=200, #odr=1, odr={Odr{N, id=-2, buy, sz=400, px=1.4}, }    [ id = -3 < 0 means this is a fake order ]
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    spdlog::debug("[test_SmartOrderBook, 1] SmartOrderBook when trade leads: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    SmartOB.applyMessage( o7 );
    SmartOB.applyMessage( s7 );
    SmartOB.applyMessage( o8 );
    SmartOB.applyMessage( s8 );
    SmartOB.applyMessage( o9 );
    SmartOB.applyMessage( s9 );
    SmartOB.applyMessage( o10 );
    SmartOB.applyMessage( s10 );
    SmartOB.applyMessage( o11 );
    SmartOB.applyMessage( s11 );

    
    /** Catched up == Order book SHOULD BE THE FOLLWING RIGHT NOW
     * L3Lvl{px=1.5, qty=100, #odr=1, odr={Odr{N, id=0, sell, sz=100, px=1.5}, }
     * L3Lvl{px=1.4, qty=200, #odr=1, odr={Odr{N, id=2, sell, sz=200, px=1.4}, }
     * --^ ASK SIDE--------BID SIDE V---
     * L3Lvl{px=1.3, qty=150, #odr=2, odr={Odr{N, id=3, buy, sz=50, px=1.3}, Odr{N, id=4, buy, sz=100, px=1.3}, }
     * L3Lvl{px=1.2, qty=100, #odr=1, odr={Odr{N, id=5, buy, sz=100, px=1.2}, }
     */

    spdlog::debug("[test_SmartOrderBook, 1] Constructed SmartOrderBook: \n{}",
                    SmartOB.getLeaderBook()->toString()
                 );

    std::vector<sob::Order> orders { o1, o2, o3, o4, o5, o6, o7, o8, o9, o10, o11 };
    sob::L3Book ob{ orders };

    REQUIRE( SmartOB.getLeaderBook()->toString() == ob.toString() );

    spdlog::set_level(spdlog::level::info);
}
