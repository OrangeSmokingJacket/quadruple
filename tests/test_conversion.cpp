#include <catch2/catch_all.hpp>

#include "quadruple.hpp"
#include "utils.hpp"
#include <cstring>
#include <numbers>

// There are some warnings emerged from Catch2 macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

TEMPLATE_TEST_CASE("same type conversion", "[conversion]", float, double) {
    auto check_conversion = [](TestType val) {
        quadruple val_converted{val};
        TestType converted_back{val_converted};
        // compare bits
        REQUIRE(std::memcmp(&val, &converted_back, sizeof(TestType)) == 0);
    };

    SECTION("positive") {
        SECTION("zero") {
            check_conversion(TestType{});
        }
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
        SECTION("infinity") {
            check_conversion(std::numeric_limits<TestType>::infinity());
        }
        SECTION("qNaN") {
            check_conversion(std::numeric_limits<TestType>::quiet_NaN());
        }
        SECTION("sNaN") {
            check_conversion(std::numeric_limits<TestType>::signaling_NaN());
        }
    }

    SECTION("negative") {
        SECTION("zero") {
            check_conversion(-TestType{});
        }
        SECTION("pi") {
            check_conversion(-std::numbers::pi_v<TestType>);
        }
        SECTION("e") {
            check_conversion(-std::numbers::e_v<TestType>);
        }
        SECTION("phi") {
            check_conversion(-std::numbers::phi_v<TestType>);
        }
        SECTION("sqrt2") {
            check_conversion(-std::numbers::sqrt2_v<TestType>);
        }
        SECTION("sqrt3") {
            check_conversion(-std::numbers::sqrt3_v<TestType>);
        }
        SECTION("inv_sqrtpi") {
            check_conversion(-std::numbers::inv_sqrtpi_v<TestType>);
        }
        SECTION("infinity") {
            check_conversion(-std::numeric_limits<TestType>::infinity());
        }
        SECTION("qNaN") {
            check_conversion(-std::numeric_limits<TestType>::quiet_NaN());
        }
        SECTION("sNaN") {
            check_conversion(-std::numeric_limits<TestType>::signaling_NaN());
        }
    }
}

TEMPLATE_TEST_CASE_SIG("different type conversion", "[conversion]",
    ((typename FromType, typename ToType), FromType, ToType), (float, double), (double, float)) {
    auto check_conversion = [](FromType val) {
        quadruple val_converted{val};
        auto converted = static_cast<ToType>(val);
        auto double_converted = static_cast<ToType>(val_converted);
        // compare bits
        REQUIRE(std::memcmp(&converted, &double_converted, sizeof(converted)) == 0);
    };
    // static_cast float::sNaN to double != double::sNaN
    auto check_NaN_conversion = [](FromType val) {
        REQUIRE(std::isnan(val));
        quadruple val_converted{val};
        ToType converted;
        if (is_qNaN(val)) {
            converted = std::numeric_limits<ToType>::quiet_NaN();
        } else if (is_sNaN(val)) {
            converted = std::numeric_limits<ToType>::signaling_NaN();
        } else {
            REQUIRE(false);
        }
        if (std::signbit(val)) {
            converted = -converted;
        }
        auto double_converted = static_cast<ToType>(val_converted);
        // compare bits
        REQUIRE(std::memcmp(&converted, &double_converted, sizeof(converted)) == 0);
    };
    SECTION("positive") {
        SECTION("zero") {
            check_conversion(FromType{});
        }
        SECTION("pi") {
            check_conversion(std::numbers::pi_v<FromType>);
        }
        SECTION("e") {
            check_conversion(std::numbers::e_v<FromType>);
        }
        SECTION("phi") {
            check_conversion(std::numbers::phi_v<FromType>);
        }
        SECTION("sqrt2") {
            check_conversion(std::numbers::sqrt2_v<FromType>);
        }
        SECTION("sqrt3") {
            check_conversion(std::numbers::sqrt3_v<FromType>);
        }
        SECTION("inv_sqrt(pi)") {
            check_conversion(std::numbers::inv_sqrtpi_v<FromType>);
        }
        SECTION("infinity") {
            check_conversion(std::numeric_limits<FromType>::infinity());
        }
        SECTION("qNaN") {
            check_NaN_conversion(std::numeric_limits<FromType>::quiet_NaN());
        }
        SECTION("sNaN") {
            check_NaN_conversion(std::numeric_limits<FromType>::signaling_NaN());
        }
    }
    SECTION("negative") {
        SECTION("zero") {
            check_conversion(-FromType{});
        }
        SECTION("pi") {
            check_conversion(-std::numbers::pi_v<FromType>);
        }
        SECTION("e") {
            check_conversion(-std::numbers::e_v<FromType>);
        }
        SECTION("phi") {
            check_conversion(-std::numbers::phi_v<FromType>);
        }
        SECTION("sqrt2") {
            check_conversion(-std::numbers::sqrt2_v<FromType>);
        }
        SECTION("sqrt3") {
            check_conversion(-std::numbers::sqrt3_v<FromType>);
        }
        SECTION("inv_sqrt(pi)") {
            check_conversion(-std::numbers::inv_sqrtpi_v<FromType>);
        }
        SECTION("infinity") {
            check_conversion(-std::numeric_limits<FromType>::infinity());
        }
        SECTION("qNaN") {
            check_NaN_conversion(-std::numeric_limits<FromType>::quiet_NaN());
        }
        SECTION("sNaN") {
            check_NaN_conversion(-std::numeric_limits<FromType>::signaling_NaN());
        }
    }
}

#pragma GCC diagnostic pop