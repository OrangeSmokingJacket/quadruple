#include <catch2/catch_all.hpp>
#include "test_helpers/test_utils.hpp"
#include "quadruple.hpp"
#include <random>

#include "ub_consistency.hpp"

template <typename T>
bool is_negative(T val) {
    return std::signbit(val);
}

template <>
bool is_negative(quadruple val) {
    return val.signbit();
}
TEMPLATE_TEST_CASE("UB consistency", "[utils]",
    int8_t,
    int16_t,
    int32_t,
    int64_t,
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t) {
    SECTION("positive") {
        SECTION("overflow") {
            double val = std::numeric_limits<double>::max();
            TestType converted = static_cast<TestType>(val);
            REQUIRE(converted == UB_handle::to_integer_conversion::POS_OVERFLOW<TestType>);
        }
        SECTION("infinity") {
            double val = std::numeric_limits<double>::infinity();
            TestType converted = static_cast<TestType>(val);
            REQUIRE(converted == UB_handle::to_integer_conversion::POS_INF<TestType>);
        }
        SECTION("NaN") {
            SECTION("quiet") {
                double val = std::numeric_limits<double>::quiet_NaN();
                TestType converted = static_cast<TestType>(val);
                REQUIRE(converted == UB_handle::to_integer_conversion::NaN<TestType>);
            }
            SECTION("signaling") {
                double val = std::numeric_limits<double>::signaling_NaN();
                TestType converted = static_cast<TestType>(val);
                REQUIRE(converted == UB_handle::to_integer_conversion::NaN<TestType>);
            }
        }
    }

    SECTION("negative") {
        SECTION("overflow") {
            double val = -std::numeric_limits<double>::max();
            TestType converted = static_cast<TestType>(val);
            REQUIRE(converted == UB_handle::to_integer_conversion::NEG_OVERFLOW<TestType>);
        }
        SECTION("infinity") {
            double val = -std::numeric_limits<double>::infinity();
            TestType converted = static_cast<TestType>(val);
            REQUIRE(converted == UB_handle::to_integer_conversion::NEG_INF<TestType>);
        }
        SECTION("NaN") {
            SECTION("quiet") {
                double val = -std::numeric_limits<double>::quiet_NaN();
                TestType converted = static_cast<TestType>(val);
                REQUIRE(converted == UB_handle::to_integer_conversion::NaN<TestType>);
            }
            SECTION("signaling") {
                double val = -std::numeric_limits<double>::signaling_NaN();
                TestType converted = static_cast<TestType>(val);
                REQUIRE(converted == UB_handle::to_integer_conversion::NaN<TestType>);
            }
        }
    }
}

TEMPLATE_TEST_CASE("test utils", "[utils]", float, double, quadruple) {
    SECTION("test repeatability") {
        SECTION("normal") {
            auto first_gen = generate_normal_numbers<TestType>(test_size);
            auto second_gen = generate_normal_numbers<TestType>(test_size);

            // check that bits are the same
            REQUIRE(std::memcmp(first_gen.data(), second_gen.data(), sizeof(TestType) * test_size) == 0);
            // all test values have to have the same sign
            for (auto val : first_gen) {
                REQUIRE_FALSE(is_negative(val));
            }
        }
        SECTION("subnormal") {
            auto first_gen = generate_subnormal_numbers<TestType>(test_size);
            auto second_gen = generate_subnormal_numbers<TestType>(test_size);

            // check that bits are the same
            REQUIRE(std::memcmp(first_gen.data(), second_gen.data(), sizeof(TestType) * test_size) == 0);
            // all test values have to have the same sign
            for (auto val : first_gen) {
                REQUIRE_FALSE(is_negative(val));
            }
        }
    }
}