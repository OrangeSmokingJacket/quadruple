#include "quadruple.hpp"
#include "test_helpers/test_utils.hpp"
#include <catch2/catch_all.hpp>
#include <random>

// Primary goal of this test is to force compile time evaluation of functions that are advertised as constexpr

// TODO: For now it is not guaranteed that non constexpr call will be executed at runtime
// But we can guarantee that compile time execution where we want it

template <quadruple (*Func)()>
struct validator1_t {
    static void check() {
        INFO("creation");
        static constexpr quadruple check1 = Func();
        quadruple check2 = Func();
        REQUIRE(std::memcmp(&check1, &check2, sizeof(quadruple)) == 0);

        INFO("default copy");
        static constexpr quadruple check1_copy1{check1};
        static constexpr quadruple check1_copy2 = check1;
        quadruple check2_copy1{check2};
        quadruple check2_copy2 = check2;
        REQUIRE(std::memcmp(&check1_copy1, &check2_copy1, sizeof(quadruple)) == 0);
        REQUIRE(std::memcmp(&check1_copy2, &check2_copy2, sizeof(quadruple)) == 0);

        INFO("default move");
        static constexpr quadruple check1_moved1{std::move(check1_copy1)};
        static constexpr quadruple check1_moved2 = std::move(check1_copy2);
        quadruple check2_moved1{std::move(check1_copy1)};
        quadruple check2_moved2 = std::move(check1_copy2);
        REQUIRE(std::memcmp(&check1_moved1, &check2_moved1, sizeof(quadruple)) == 0);
        REQUIRE(std::memcmp(&check1_moved2, &check2_moved2, sizeof(quadruple)) == 0);
    }
};

template <int32_t Exponent>
struct validator2_t {
    static void check() {
        SECTION(std::to_string(Exponent)) {
            INFO("creation");
            constexpr quadruple power1 = quadruple::power_of_2(Exponent);
            quadruple power2 = quadruple::power_of_2(Exponent);
            REQUIRE(std::memcmp(&power1, &power2, sizeof(quadruple)) == 0);

            INFO("member functions check");
            if constexpr (power1.is_zero()) {
                REQUIRE(power2.is_zero());
            } else {
                REQUIRE_FALSE(power2.is_zero());
            }

            if constexpr (power1.is_NaN()) {
                REQUIRE(power2.is_NaN());
            } else {
                REQUIRE_FALSE(power2.is_NaN());
            }

            if constexpr (power1.is_quiet_NaN()) {
                REQUIRE(power2.is_quiet_NaN());
            } else {
                REQUIRE_FALSE(power2.is_quiet_NaN());
            }

            if constexpr (power1.is_signaling_NaN()) {
                REQUIRE(power2.is_signaling_NaN());
            } else {
                REQUIRE_FALSE(power2.is_signaling_NaN());
            }

            if constexpr (power1.is_subnormal()) {
                REQUIRE(power2.is_subnormal());
            } else {
                REQUIRE_FALSE(power2.is_subnormal());
            }

            if constexpr (power1.signbit()) {
                REQUIRE(power2.signbit());
            } else {
                REQUIRE_FALSE(power2.signbit());
            }
        }
    }
};

template <template <typename> typename Comp>
static consteval bool compare(const quadruple& lhs, const quadruple& rhs) {
    constexpr Comp<quadruple> comparator{};
    return comparator(lhs, rhs);
}
template <typename Comp>
static consteval bool compare(const quadruple& lhs, const quadruple& rhs) {
    constexpr Comp comparator{};
    return comparator(lhs, rhs);
}
static consteval std::partial_ordering compare(const quadruple& lhs, const quadruple& rhs) {
    constexpr std::compare_three_way comparator{};
    return comparator(lhs, rhs);
}

using comparators = std::
    tuple<std::less<quadruple>, std::less_equal<quadruple>, std::greater<quadruple>, std::greater_equal<quadruple>>;

template <typename Comp, typename T, std::size_t N>
consteval std::array<T, N> sort_array(std::array<T, N> arr) {
    constexpr Comp comparator{};
    std::sort(arr.begin(), arr.end(), comparator);
    return arr;
}

TEST_CASE("quadruple constexpr evaluation", "[constexpr][member functions]") {
    SECTION("precomputed values") {
        SECTION("default constructor") {
            static constexpr quadruple check1{};
            quadruple check2{};
            REQUIRE(std::memcmp(&check1, &check2, sizeof(quadruple)) == 0);
        }
        SECTION("quiet_NaN") { validator1_t<&quadruple::quiet_NaN>::check(); }
        SECTION("quiet_NaN") { validator1_t<&quadruple::quiet_NaN>::check(); }
        SECTION("negative_quiet_NaN") { validator1_t<&quadruple::negative_quiet_NaN>::check(); }
        SECTION("signaling_NaN") { validator1_t<&quadruple::signaling_NaN>::check(); }
        SECTION("negative_signaling_NaN") { validator1_t<&quadruple::negative_signaling_NaN>::check(); }
        SECTION("infinity") { validator1_t<&quadruple::infinity>::check(); }
        SECTION("negative_infinity") { validator1_t<&quadruple::negative_infinity>::check(); }
        SECTION("max") { validator1_t<&quadruple::max>::check(); }
        SECTION("min") { validator1_t<&quadruple::min>::check(); }
    }

    SECTION("powers of 2") {
        static constexpr std::array<int32_t, 16> check_values = {quadruple_min_representable_pow2 - 1,
                                                                 quadruple_min_representable_pow2,
                                                                 quadruple_min_representable_pow2 + 2,
                                                                 quadruple_min_representable_pow2 + 63,
                                                                 quadruple_min_representable_pow2 + 64,
                                                                 quadruple_min_normal_representable_pow2 - 1,
                                                                 quadruple_min_normal_representable_pow2,
                                                                 quadruple_min_normal_representable_pow2 + 1,
                                                                 -64,
                                                                 -1,
                                                                 0,
                                                                 1,
                                                                 64,
                                                                 quadruple_max_representable_pow2 - 2,
                                                                 quadruple_max_representable_pow2,
                                                                 quadruple_max_representable_pow2 + 1};

        validator2_t<check_values[0]>::check();
        validator2_t<check_values[1]>::check();
        validator2_t<check_values[2]>::check();
        validator2_t<check_values[3]>::check();
        validator2_t<check_values[4]>::check();
        validator2_t<check_values[5]>::check();
        validator2_t<check_values[6]>::check();
        validator2_t<check_values[7]>::check();
        validator2_t<check_values[8]>::check();
        validator2_t<check_values[9]>::check();
        validator2_t<check_values[10]>::check();
        validator2_t<check_values[11]>::check();
        validator2_t<check_values[12]>::check();
        validator2_t<check_values[13]>::check();
        validator2_t<check_values[14]>::check();
        validator2_t<check_values[15]>::check();
    }
}

TEST_CASE("quadruple constexpr comparison", "[constexpr][comparison]") {
    static constexpr quadruple zero_plus{};
    static constexpr quadruple zero_minus = quadruple{}.flip_sign();
    static constexpr quadruple infinity_plus = quadruple::infinity();
    static constexpr quadruple infinity_minus = quadruple::negative_infinity();
    static constexpr quadruple qNaN = quadruple::quiet_NaN();
    static constexpr quadruple sNaN = quadruple::signaling_NaN();

    SECTION("equals") {
        SECTION("signed zeros") {
            SECTION("+/+") { REQUIRE(compare<std::equal_to>(zero_plus, zero_plus)); }
            SECTION("+/-") { REQUIRE(compare<std::equal_to>(zero_plus, zero_minus)); }
            SECTION("-/+") { REQUIRE(compare<std::equal_to>(zero_minus, zero_plus)); }
            SECTION("-/-") { REQUIRE(compare<std::equal_to>(zero_minus, zero_minus)); }
        }
        SECTION("NaN") {
            SECTION("+0") {
                REQUIRE_FALSE(compare<std::equal_to>(zero_plus, qNaN));
                REQUIRE_FALSE(compare<std::equal_to>(zero_plus, sNaN));
                REQUIRE_FALSE(compare<std::equal_to>(qNaN, zero_plus));
                REQUIRE_FALSE(compare<std::equal_to>(sNaN, zero_plus));
            }
            SECTION("-0") {
                REQUIRE_FALSE(compare<std::equal_to>(zero_minus, qNaN));
                REQUIRE_FALSE(compare<std::equal_to>(zero_minus, sNaN));
                REQUIRE_FALSE(compare<std::equal_to>(qNaN, zero_minus));
                REQUIRE_FALSE(compare<std::equal_to>(sNaN, zero_minus));
            }
            SECTION("+inf") {
                REQUIRE_FALSE(compare<std::equal_to>(infinity_plus, qNaN));
                REQUIRE_FALSE(compare<std::equal_to>(infinity_plus, sNaN));
                REQUIRE_FALSE(compare<std::equal_to>(qNaN, infinity_plus));
                REQUIRE_FALSE(compare<std::equal_to>(sNaN, infinity_plus));
            }
            SECTION("-inf") {
                REQUIRE_FALSE(compare<std::equal_to>(infinity_minus, qNaN));
                REQUIRE_FALSE(compare<std::equal_to>(infinity_minus, sNaN));
                REQUIRE_FALSE(compare<std::equal_to>(qNaN, infinity_minus));
                REQUIRE_FALSE(compare<std::equal_to>(sNaN, infinity_minus));
            }
            SECTION("NaN") {
                REQUIRE_FALSE(compare<std::equal_to>(qNaN, qNaN));
                REQUIRE_FALSE(compare<std::equal_to>(qNaN, sNaN));
                REQUIRE_FALSE(compare<std::equal_to>(sNaN, qNaN));
                REQUIRE_FALSE(compare<std::equal_to>(sNaN, sNaN));
            }
        }
    }

    SECTION("not equals") {
        SECTION("signed zeros") {
            SECTION("+/+") { REQUIRE_FALSE(compare<std::not_equal_to>(zero_plus, zero_plus)); }
            SECTION("+/-") { REQUIRE_FALSE(compare<std::not_equal_to>(zero_plus, zero_minus)); }
            SECTION("-/+") { REQUIRE_FALSE(compare<std::not_equal_to>(zero_minus, zero_plus)); }
            SECTION("-/-") { REQUIRE_FALSE(compare<std::not_equal_to>(zero_minus, zero_minus)); }
        }
        SECTION("NaN") {
            SECTION("+0") {
                REQUIRE(compare<std::not_equal_to>(zero_plus, qNaN));
                REQUIRE(compare<std::not_equal_to>(zero_plus, sNaN));
                REQUIRE(compare<std::not_equal_to>(qNaN, zero_plus));
                REQUIRE(compare<std::not_equal_to>(sNaN, zero_plus));
            }
            SECTION("-0") {
                REQUIRE(compare<std::not_equal_to>(zero_minus, qNaN));
                REQUIRE(compare<std::not_equal_to>(zero_minus, sNaN));
                REQUIRE(compare<std::not_equal_to>(qNaN, zero_minus));
                REQUIRE(compare<std::not_equal_to>(sNaN, zero_minus));
            }
            SECTION("+inf") {
                REQUIRE(compare<std::not_equal_to>(infinity_plus, qNaN));
                REQUIRE(compare<std::not_equal_to>(infinity_plus, sNaN));
                REQUIRE(compare<std::not_equal_to>(qNaN, infinity_plus));
                REQUIRE(compare<std::not_equal_to>(sNaN, infinity_plus));
            }
            SECTION("-inf") {
                REQUIRE(compare<std::not_equal_to>(infinity_minus, qNaN));
                REQUIRE(compare<std::not_equal_to>(infinity_minus, sNaN));
                REQUIRE(compare<std::not_equal_to>(qNaN, infinity_minus));
                REQUIRE(compare<std::not_equal_to>(sNaN, infinity_minus));
            }
            SECTION("NaN") {
                REQUIRE(compare<std::not_equal_to>(qNaN, qNaN));
                REQUIRE(compare<std::not_equal_to>(qNaN, sNaN));
                REQUIRE(compare<std::not_equal_to>(sNaN, qNaN));
                REQUIRE(compare<std::not_equal_to>(sNaN, sNaN));
            }
        }
    }

    SECTION("spaceship", "[comparison]") {
        SECTION("signed zeros") {
            SECTION("+/+") { REQUIRE(compare(zero_plus, zero_plus) == std::partial_ordering::equivalent); }
            SECTION("+/-") { REQUIRE(compare(zero_plus, zero_minus) == std::partial_ordering::equivalent); }
            SECTION("-/+") { REQUIRE(compare(zero_minus, zero_plus) == std::partial_ordering::equivalent); }
            SECTION("-/-") { REQUIRE(compare(zero_minus, zero_minus) == std::partial_ordering::equivalent); }
        }
        SECTION("NaN") {
            SECTION("+0") {
                REQUIRE(compare(zero_plus, qNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(zero_plus, sNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(qNaN, zero_plus) == std::partial_ordering::unordered);
                REQUIRE(compare(sNaN, zero_plus) == std::partial_ordering::unordered);
            }
            SECTION("-0") {
                REQUIRE(compare(zero_minus, qNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(zero_minus, sNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(qNaN, zero_minus) == std::partial_ordering::unordered);
                REQUIRE(compare(sNaN, zero_minus) == std::partial_ordering::unordered);
            }
            SECTION("+inf") {
                REQUIRE(compare(infinity_plus, qNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(infinity_plus, sNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(qNaN, infinity_plus) == std::partial_ordering::unordered);
                REQUIRE(compare(sNaN, infinity_plus) == std::partial_ordering::unordered);
            }
            SECTION("-inf") {
                REQUIRE(compare(infinity_minus, qNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(infinity_minus, sNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(qNaN, infinity_minus) == std::partial_ordering::unordered);
                REQUIRE(compare(sNaN, infinity_minus) == std::partial_ordering::unordered);
            }
            SECTION("NaN") {
                REQUIRE(compare(qNaN, qNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(qNaN, sNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(sNaN, qNaN) == std::partial_ordering::unordered);
                REQUIRE(compare(sNaN, sNaN) == std::partial_ordering::unordered);
            }
        }
    }
}

TEMPLATE_LIST_TEST_CASE("quadruple constexpr comparisons less and derivatives", "[comparison]", comparators) {
    SECTION("constants") {
        static constexpr std::array<quadruple, 27> values{
            quadruple::infinity(),
            quadruple::negative_infinity(),
            quadruple::power_of_2(quadruple_min_representable_pow2),
            quadruple::power_of_2(quadruple_min_representable_pow2).flip_sign(),
            quadruple::power_of_2(quadruple_min_normal_representable_pow2 - 10),
            quadruple::power_of_2(quadruple_min_normal_representable_pow2 - 10).flip_sign(),
            quadruple::power_of_2(quadruple_min_normal_representable_pow2),
            quadruple::power_of_2(quadruple_min_normal_representable_pow2).flip_sign(),
            quadruple::power_of_2(-1280),
            quadruple::power_of_2(-1280).flip_sign(),
            quadruple::power_of_2(-128),
            quadruple::power_of_2(-128).flip_sign(),
            quadruple::power_of_2(-4),
            quadruple::power_of_2(-4).flip_sign(),
            quadruple::power_of_2(-1),
            quadruple::power_of_2(-1).flip_sign(),
            quadruple::power_of_2(0),
            quadruple::power_of_2(1),
            quadruple::power_of_2(1).flip_sign(),
            quadruple::power_of_2(4),
            quadruple::power_of_2(4).flip_sign(),
            quadruple::power_of_2(128),
            quadruple::power_of_2(128).flip_sign(),
            quadruple::power_of_2(1280),
            quadruple::power_of_2(1280).flip_sign(),
            quadruple::power_of_2(quadruple_max_representable_pow2),
            quadruple::power_of_2(quadruple_max_representable_pow2).flip_sign()};

        // Sort
        constexpr auto sorted_array = sort_array<TestType>(values);
        // Verify results
        REQUIRE(compare<TestType>(sorted_array[0], sorted_array[1]));
        REQUIRE(compare<TestType>(sorted_array[1], sorted_array[2]));
        REQUIRE(compare<TestType>(sorted_array[2], sorted_array[3]));
        REQUIRE(compare<TestType>(sorted_array[3], sorted_array[4]));
        REQUIRE(compare<TestType>(sorted_array[4], sorted_array[5]));
        REQUIRE(compare<TestType>(sorted_array[5], sorted_array[6]));
        REQUIRE(compare<TestType>(sorted_array[6], sorted_array[7]));
        REQUIRE(compare<TestType>(sorted_array[7], sorted_array[8]));
        REQUIRE(compare<TestType>(sorted_array[8], sorted_array[9]));
        REQUIRE(compare<TestType>(sorted_array[9], sorted_array[10]));
        REQUIRE(compare<TestType>(sorted_array[10], sorted_array[11]));
        REQUIRE(compare<TestType>(sorted_array[11], sorted_array[12]));
        REQUIRE(compare<TestType>(sorted_array[12], sorted_array[13]));
        REQUIRE(compare<TestType>(sorted_array[13], sorted_array[14]));
        REQUIRE(compare<TestType>(sorted_array[14], sorted_array[15]));
        REQUIRE(compare<TestType>(sorted_array[15], sorted_array[16]));
        REQUIRE(compare<TestType>(sorted_array[16], sorted_array[17]));
        REQUIRE(compare<TestType>(sorted_array[17], sorted_array[18]));
        REQUIRE(compare<TestType>(sorted_array[18], sorted_array[19]));
        REQUIRE(compare<TestType>(sorted_array[19], sorted_array[20]));
        REQUIRE(compare<TestType>(sorted_array[20], sorted_array[21]));
        REQUIRE(compare<TestType>(sorted_array[21], sorted_array[22]));
        REQUIRE(compare<TestType>(sorted_array[22], sorted_array[23]));
        REQUIRE(compare<TestType>(sorted_array[23], sorted_array[24]));
        REQUIRE(compare<TestType>(sorted_array[24], sorted_array[25]));
        REQUIRE(compare<TestType>(sorted_array[25], sorted_array[26]));
    }
}
