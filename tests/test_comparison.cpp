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
            REQUIRE_FALSE(quadruple{} == quadruple::nan());
            REQUIRE_FALSE(quadruple::nan() == quadruple{});
        }
        SECTION("-0") {
            REQUIRE_FALSE((-quadruple{}) == quadruple::nan());
            REQUIRE_FALSE(quadruple::nan() == (-quadruple{}));
        }
        SECTION("+inf") {
            REQUIRE_FALSE(quadruple::infinity() == quadruple::nan());
            REQUIRE_FALSE(quadruple::nan() == quadruple::infinity());
        }
        SECTION("-inf") {
            REQUIRE_FALSE((-quadruple::infinity()) == quadruple::nan());
            REQUIRE_FALSE(quadruple::nan() == (-quadruple::infinity()));
        }
        SECTION("NaN") {
            REQUIRE_FALSE(quadruple::nan() == quadruple::nan());
        }
    }
}


#pragma GCC diagnostic pop