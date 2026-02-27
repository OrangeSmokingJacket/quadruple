#include <catch2/catch.hpp>

#include "quadruple.hpp"
#include "utils.hpp"
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

TEMPLATE_TEST_CASE("conversion", "[template]", float, double) {
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

TEMPLATE_PRODUCT_TEST_CASE("convert through quadruple", "[first_type][second_type]", test_type_pair, ((float, double), (double, float))) {
    auto check_conversion = [](typename TestType::T1 val) {
        quadruple val_converted{val};
        auto converted = static_cast<TestType::T2>(val);
        auto double_converted = static_cast<TestType::T2>(val_converted);
        // compare bits
        REQUIRE(std::memcmp(&converted, &double_converted, sizeof(converted)) == 0);
    };
    // static_cast float::sNaN to double != double::sNaN
    auto check_NaN_conversion = [](typename TestType::T1 val) {
        REQUIRE(std::isnan(val));
        quadruple val_converted{val};
        typename TestType::T2 converted;
        if (is_qNaN(val)) {
            converted = std::numeric_limits<typename TestType::T2>::quiet_NaN();
        } else if (is_sNaN(val)) {
            converted = std::numeric_limits<typename TestType::T2>::signaling_NaN();
        } else {
            REQUIRE(false);
        }
        if (std::signbit(val)) {
            converted = -converted;
        }
        auto double_converted = static_cast<TestType::T2>(val_converted);
        // compare bits
        REQUIRE(std::memcmp(&converted, &double_converted, sizeof(converted)) == 0);
    };
    SECTION("positive") {
        SECTION("zero") {
            check_conversion(typename TestType::T1{});
        }
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
        SECTION("infinity") {
            check_conversion(std::numeric_limits<typename TestType::T1>::infinity());
        }
        SECTION("qNaN") {
            check_NaN_conversion(std::numeric_limits<typename TestType::T1>::quiet_NaN());
        }
        SECTION("sNaN") {
            check_NaN_conversion(std::numeric_limits<typename TestType::T1>::signaling_NaN());
        }
    }
    SECTION("negative") {
        SECTION("zero") {
            check_conversion(-typename TestType::T1{});
        }
        SECTION("pi") {
            check_conversion(-std::numbers::pi_v<typename TestType::T1>);
        }
        SECTION("e") {
            check_conversion(-std::numbers::e_v<typename TestType::T1>);
        }
        SECTION("phi") {
            check_conversion(-std::numbers::phi_v<typename TestType::T1>);
        }
        SECTION("sqrt2") {
            check_conversion(-std::numbers::sqrt2_v<typename TestType::T1>);
        }
        SECTION("sqrt3") {
            check_conversion(-std::numbers::sqrt3_v<typename TestType::T1>);
        }
        SECTION("inv_sqrt(pi)") {
            check_conversion(-std::numbers::inv_sqrtpi_v<typename TestType::T1>);
        }
        SECTION("infinity") {
            check_conversion(-std::numeric_limits<typename TestType::T1>::infinity());
        }
        SECTION("qNaN") {
            check_NaN_conversion(-std::numeric_limits<typename TestType::T1>::quiet_NaN());
        }
        SECTION("sNaN") {
            check_NaN_conversion(-std::numeric_limits<typename TestType::T1>::signaling_NaN());
        }
    }
}

#pragma GCC diagnostic pop