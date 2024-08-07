#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#include <Order.h>



TEST_CASE("test_Order", "1")
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("test_Order testing...");
    REQUIRE(1 == 1);
}





