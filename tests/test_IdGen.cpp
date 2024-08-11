#define CATCH_CONFIG_MAIN
#include <IdGen.h>
#include <catch2/catch_all.hpp>


TEST_CASE( "test_id_gen", "1" )
{
    REQUIRE( sob::IdGen::genId() == 0 );
    REQUIRE( sob::IdGen::genId() == 1 );
    REQUIRE( sob::IdGen::genId() == 2 );
    REQUIRE( sob::IdGen::genId() == 3 );
    REQUIRE( sob::IdGen::genId() == 4 );

    // testing reset
    sob::IdGen::getInstance().reset();
    REQUIRE( sob::IdGen::genId() == 0 );
    REQUIRE( sob::IdGen::genId() == 1 );
    REQUIRE( sob::IdGen::genId() == 2 );
    REQUIRE( sob::IdGen::genId() == 3 );
    REQUIRE( sob::IdGen::genId() == 4 );
}

TEST_CASE( "test_id_gen_neg", "1" )
{
    REQUIRE( sob::IdGenNeg::genId() == -1 );
    REQUIRE( sob::IdGenNeg::genId() == -2 );
    REQUIRE( sob::IdGenNeg::genId() == -3 );
    REQUIRE( sob::IdGenNeg::genId() == -4 );
    REQUIRE( sob::IdGenNeg::genId() == -5 );

    // testing reset
    sob::IdGenNeg::getInstance().reset();
    REQUIRE( sob::IdGenNeg::genId() == -1 );
    REQUIRE( sob::IdGenNeg::genId() == -2 );
    REQUIRE( sob::IdGenNeg::genId() == -3 );
    REQUIRE( sob::IdGenNeg::genId() == -4 );
    REQUIRE( sob::IdGenNeg::genId() == -5 );
}
