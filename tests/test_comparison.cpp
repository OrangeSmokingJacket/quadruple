#include <cstring>
#include <catch2/catch_all.hpp>

#include "quadruple.hpp"

TEST_CASE("equals", "[comparison]") {
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

TEST_CASE("not equals", "[comparison]") {
    SECTION("signed zeros") {
        SECTION("+/+") {
            REQUIRE_FALSE(quadruple{} != quadruple{});
        }
        SECTION("+/-") {
            REQUIRE_FALSE(quadruple{} != (-quadruple{}));
        }
        SECTION("-/+") {
            REQUIRE_FALSE((-quadruple{}) != quadruple{});
        }
        SECTION("-/-") {
            REQUIRE_FALSE((-quadruple{}) != (-quadruple{}));
        }
    }
    SECTION("NaN") {
        SECTION("+0") {
            REQUIRE(quadruple{} != quadruple::quiet_NaN());
            REQUIRE(quadruple{} != quadruple::signaling_NaN());
            REQUIRE(quadruple::quiet_NaN() != quadruple{});
            REQUIRE(quadruple::signaling_NaN() != quadruple{});
        }
        SECTION("-0") {
            REQUIRE((-quadruple{}) != quadruple::quiet_NaN());
            REQUIRE((-quadruple{}) != quadruple::signaling_NaN());
            REQUIRE(quadruple::quiet_NaN() != (-quadruple{}));
            REQUIRE(quadruple::signaling_NaN() != (-quadruple{}));
        }
        SECTION("+inf") {
            REQUIRE(quadruple::infinity() != quadruple::quiet_NaN());
            REQUIRE(quadruple::infinity() != quadruple::signaling_NaN());
            REQUIRE(quadruple::quiet_NaN() != quadruple::infinity());
            REQUIRE(quadruple::signaling_NaN() != quadruple::infinity());
        }
        SECTION("-inf") {
            REQUIRE((-quadruple::infinity()) != quadruple::quiet_NaN());
            REQUIRE((-quadruple::infinity()) != quadruple::signaling_NaN());
            REQUIRE(quadruple::quiet_NaN() != (-quadruple::infinity()));
            REQUIRE(quadruple::signaling_NaN() != (-quadruple::infinity()));
        }
        SECTION("NaN") {
            REQUIRE(quadruple::quiet_NaN() != quadruple::quiet_NaN());
            REQUIRE(quadruple::quiet_NaN() != quadruple::signaling_NaN());
            REQUIRE(quadruple::signaling_NaN() != quadruple::quiet_NaN());
            REQUIRE(quadruple::signaling_NaN() != quadruple::signaling_NaN());
        }
    }
}

TEMPLATE_TEST_CASE_SIG("less and derivatives", "[comparison]",
    ((typename ValueType, typename ComparatorType), ValueType, ComparatorType),
    (float, std::less<>),
    (float, std::less_equal<>),
    (float, std::greater<>),
    (float, std::greater_equal<>),
    (double, std::less<>),
    (double, std::less_equal<>),
    (double, std::greater<>),
    (double, std::greater_equal<>)) {

    std::vector<ValueType> values;
    std::vector<quadruple> converted;

    // Fill vector
    // zeros are equal no matter the sign, and NaN are not ordered, so they are absent from the test
    values.emplace_back(ValueType{});
    values.emplace_back(std::numbers::pi_v<ValueType>);
    values.emplace_back(-std::numbers::pi_v<ValueType>);
    values.emplace_back(std::numbers::e_v<ValueType>);
    values.emplace_back(-std::numbers::e_v<ValueType>);
    values.emplace_back(std::numbers::phi_v<ValueType>);
    values.emplace_back(-std::numbers::phi_v<ValueType>);
    values.emplace_back(std::numbers::sqrt2_v<ValueType>);
    values.emplace_back(-std::numbers::sqrt2_v<ValueType>);
    values.emplace_back(std::numbers::sqrt3_v<ValueType>);
    values.emplace_back(-std::numbers::sqrt3_v<ValueType>);
    values.emplace_back(std::numbers::inv_sqrtpi_v<ValueType>);
    values.emplace_back(-std::numbers::inv_sqrtpi_v<ValueType>);
    values.emplace_back(std::numeric_limits<ValueType>::infinity());
    values.emplace_back(-std::numeric_limits<ValueType>::infinity());
    for (auto value : values) {
        converted.emplace_back(value);
    }

    // Sort both versions
    ComparatorType comparator{};
    std::sort(values.begin(), values.end(), comparator);
    std::sort(converted.begin(), converted.end(), comparator);

    // Compare results
    for (size_t i = 0; i < values.size(); i++) {
        ValueType converted_back{converted[i]};
        REQUIRE(std::memcmp(&converted_back, &(values[i]), sizeof(ValueType)) == 0);
        quadruple converted_value{values[i]};
        REQUIRE(converted_value == converted[i]);
        REQUIRE(std::memcmp(&converted_value, &(converted[i]), sizeof(quadruple)) == 0);
    }
}