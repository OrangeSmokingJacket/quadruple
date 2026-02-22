#include <catch2/catch.hpp>

#include "quadruple.hpp"
#include <cstring>
#include <numbers>

template <typename T, typename U>
struct test_type_pair {
    using T1 = T;
    using T2 = U;
};

// There are some warnings emerged from Catch2 macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

// TODO: add tests for NaN and infinity
// TODO: subtraction
TEMPLATE_TEST_CASE("addition", "[template]", float, double) {
    auto check_addition = [](TestType val) {
        TestType result = val + val;
        quadruple val_converted{val};
        TestType converted_back{val_converted + val_converted};
        // compare bits
        REQUIRE(std::memcmp(&result, &converted_back, sizeof(TestType)) == 0);
    };

    SECTION("positive") {
        SECTION("pi") {
            check_addition(std::numbers::pi_v<TestType>);
        }
        SECTION("e") {
            check_addition(std::numbers::e_v<TestType>);
        }
        SECTION("phi") {
            check_addition(std::numbers::phi_v<TestType>);
        }
        SECTION("sqrt2") {
            check_addition(std::numbers::sqrt2_v<TestType>);
        }
        SECTION("sqrt3") {
            check_addition(std::numbers::sqrt3_v<TestType>);
        }
        SECTION("inv_sqrtpi") {
            check_addition(std::numbers::inv_sqrtpi_v<TestType>);
        }
    }
    SECTION("negative") {
        SECTION("pi") {
            check_addition(-std::numbers::pi_v<TestType>);
        }
        SECTION("e") {
            check_addition(-std::numbers::e_v<TestType>);
        }
        SECTION("phi") {
            check_addition(-std::numbers::phi_v<TestType>);
        }
        SECTION("sqrt2") {
            check_addition(-std::numbers::sqrt2_v<TestType>);
        }
        SECTION("sqrt3") {
            check_addition(-std::numbers::sqrt3_v<TestType>);
        }
        SECTION("inv_sqrtpi") {
            check_addition(-std::numbers::inv_sqrtpi_v<TestType>);
        }
    }
}

#pragma GCC diagnostic pop