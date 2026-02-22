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
TEMPLATE_TEST_CASE("conversion", "[template]", float, double) {
    SECTION("bounded") {
        auto check_conversion = [](TestType val) {
            quadruple val_converted{val};
            TestType converted_back{val_converted};
            // compare bits
            REQUIRE(std::memcmp(&val, &converted_back, sizeof(TestType)) == 0);
        };

        SECTION("pi") {
            check_conversion(std::numbers::pi_v<TestType>);
        }
        SECTION("e") {
            check_conversion(std::numbers::e_v<TestType>);
        }
        SECTION("phi") {
            check_conversion(std::numbers::phi_v<TestType>);
        }
        SECTION("sqrt2") {
            check_conversion(std::numbers::sqrt2_v<TestType>);
        }
        SECTION("sqrt3") {
            check_conversion(std::numbers::sqrt3_v<TestType>);
        }
        SECTION("inv_sqrtpi") {
            check_conversion(std::numbers::inv_sqrtpi_v<TestType>);
        }
    }
}

TEMPLATE_PRODUCT_TEST_CASE("convert through quadruple", "[first_type][second_type]", test_type_pair, ((float, double), (double, float))) {
    SECTION("bounded") {
        auto check_conversion = [](typename TestType::T1 val) {
            quadruple val_converted{val};
            auto converted = static_cast<TestType::T2>(val);
            auto double_converted = static_cast<TestType::T2>(val_converted);
            // compare bits
            REQUIRE(std::memcmp(&converted, &double_converted, sizeof(converted)) == 0);
        };

        SECTION("pi") {
            check_conversion(std::numbers::pi_v<typename TestType::T1>);
        }
        SECTION("e") {
            check_conversion(std::numbers::e_v<typename TestType::T1>);
        }
        SECTION("phi") {
            check_conversion(std::numbers::phi_v<typename TestType::T1>);
        }
        SECTION("sqrt2") {
            check_conversion(std::numbers::sqrt2_v<typename TestType::T1>);
        }
        SECTION("sqrt3") {
            check_conversion(std::numbers::sqrt3_v<typename TestType::T1>);
        }
        SECTION("inv_sqrt(pi)") {
            check_conversion(std::numbers::inv_sqrtpi_v<typename TestType::T1>);
        }
    }
}

#pragma GCC diagnostic pop