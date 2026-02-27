#include <catch2/catch.hpp>

#include "quadruple.hpp"

// There are some warnings emerged from Catch2 macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

TEST_CASE("equals") {
    SECTION("signed zeros") {
        SECTION("+/+") {
            REQUIRE(quadruple{} == quadruple{});
        }
        SECTION("+/-") {
            REQUIRE(quadruple{} == (-quadruple{}));
        }
        SECTION("-/+") {
            REQUIRE((-quadruple{}) == quadruple{});
        }
        SECTION("-/-") {
            REQUIRE((-quadruple{}) == (-quadruple{}));
        }
    }
    SECTION("NaN") {
        SECTION("+0") {
            REQUIRE_FALSE(quadruple{} == quadruple::quiet_NaN());
            REQUIRE_FALSE(quadruple{} == quadruple::signaling_NaN());
            REQUIRE_FALSE(quadruple::quiet_NaN() == quadruple{});
            REQUIRE_FALSE(quadruple::signaling_NaN() == quadruple{});
        }
        SECTION("-0") {
            REQUIRE_FALSE((-quadruple{}) == quadruple::quiet_NaN());
            REQUIRE_FALSE((-quadruple{}) == quadruple::signaling_NaN());
            REQUIRE_FALSE(quadruple::quiet_NaN() == (-quadruple{}));
            REQUIRE_FALSE(quadruple::signaling_NaN() == (-quadruple{}));
        }
        SECTION("+inf") {
            REQUIRE_FALSE(quadruple::infinity() == quadruple::quiet_NaN());
            REQUIRE_FALSE(quadruple::infinity() == quadruple::signaling_NaN());
            REQUIRE_FALSE(quadruple::quiet_NaN() == quadruple::infinity());
            REQUIRE_FALSE(quadruple::signaling_NaN() == quadruple::infinity());
        }
        SECTION("-inf") {
            REQUIRE_FALSE((-quadruple::infinity()) == quadruple::quiet_NaN());
            REQUIRE_FALSE((-quadruple::infinity()) == quadruple::signaling_NaN());
            REQUIRE_FALSE(quadruple::quiet_NaN() == (-quadruple::infinity()));
            REQUIRE_FALSE(quadruple::signaling_NaN() == (-quadruple::infinity()));
        }
        SECTION("NaN") {
            REQUIRE_FALSE(quadruple::quiet_NaN() == quadruple::quiet_NaN());
            REQUIRE_FALSE(quadruple::quiet_NaN() == quadruple::signaling_NaN());
            REQUIRE_FALSE(quadruple::signaling_NaN() == quadruple::quiet_NaN());
            REQUIRE_FALSE(quadruple::signaling_NaN() == quadruple::signaling_NaN());
        }
    }
}


#pragma GCC diagnostic pop