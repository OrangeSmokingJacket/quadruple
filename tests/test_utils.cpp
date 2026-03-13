#include <catch2/catch_all.hpp>
#include "test_helpers/test_utils.hpp"
#include "quadruple.hpp"
#include <random>

template <typename T>
bool is_negative(T val) {
    return std::signbit(val);
}

template <>
bool is_negative(quadruple val) {
    return val.signbit();
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