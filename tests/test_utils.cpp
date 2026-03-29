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

TEMPLATE_LIST_TEST_CASE("UB consistency", "[utils]", integer_types) {
    SECTION("positive") {
        SECTION("overflow") {
            double val = std::numeric_limits<double>::max();
            TestType converted = static_cast<TestType>(val);
            REQUIRE(converted == UB_handle::to_integer_conversion::POS_OVERFLOW<TestType>);
        }
        SECTION("infinity") {
            double val = std::numeric_limits<double>::infinity();
            TestType converted = static_cast<TestType>(val);
            REQUIRE(converted == UB_handle::to_integer_conversion::POS_OVERFLOW<TestType>);
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
            REQUIRE(converted == UB_handle::to_integer_conversion::NEG_OVERFLOW<TestType>);
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

template <typename...>
struct custom_type_packer {};

using dummy_type = void*;

// Catch2 only excepts fully instantiated types, so we pass some instantiation and rebind it inside
TEMPLATE_TEST_CASE("types cross product", "[utils]", std::tuple<dummy_type>, custom_type_packer<dummy_type>) {
    SECTION("equal count") {
        STATIC_REQUIRE(std::is_same_v<
                           typename types_cross_product<rebind_with<TestType, int, char>, rebind_with<TestType, bool, std::vector<int>>>::type,
                           rebind_with<TestType,
                               rebind_with<TestType, int, bool>,
                               rebind_with<TestType, int, std::vector<int>>,
                               rebind_with<TestType, char, bool>,
                               rebind_with<TestType, char, std::vector<int>>
                           >
                       >);
    }

    SECTION("left is bigger") {
        STATIC_REQUIRE(std::is_same_v<
                           typename types_cross_product<rebind_with<TestType, int, char, bool>, rebind_with<TestType, bool, std::vector<int>>>::type,
                           rebind_with<TestType,
                               rebind_with<TestType, int, bool>,
                               rebind_with<TestType, int, std::vector<int>>,
                               rebind_with<TestType, char, bool>,
                               rebind_with<TestType, char, std::vector<int>>,
                               rebind_with<TestType, bool, bool>,
                               rebind_with<TestType, bool, std::vector<int>>
                           >
                       >);
    }

    SECTION("right is bigger") {
        STATIC_REQUIRE(std::is_same_v<
                           typename types_cross_product<rebind_with<TestType, int, char>, rebind_with<TestType, bool, std::vector<int>, std::string>>::type,
                           rebind_with<TestType,
                               rebind_with<TestType, int, bool>,
                               rebind_with<TestType, int, std::vector<int>>,
                               rebind_with<TestType, int, std::string>,
                               rebind_with<TestType, char, bool>,
                               rebind_with<TestType, char, std::vector<int>>,
                               rebind_with<TestType, char, std::string>
                           >
                       >);
    }
}