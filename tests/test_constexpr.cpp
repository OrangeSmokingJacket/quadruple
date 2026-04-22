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

template <quadruple (*Func)(int32_t), int32_t Exponent>
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

TEST_CASE("quadruple constexpr evaluation", "[constexpr]") {
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

        validator2_t<&quadruple::power_of_2, check_values[0]>::check();
        validator2_t<&quadruple::power_of_2, check_values[1]>::check();
        validator2_t<&quadruple::power_of_2, check_values[2]>::check();
        validator2_t<&quadruple::power_of_2, check_values[3]>::check();
        validator2_t<&quadruple::power_of_2, check_values[4]>::check();
        validator2_t<&quadruple::power_of_2, check_values[5]>::check();
        validator2_t<&quadruple::power_of_2, check_values[6]>::check();
        validator2_t<&quadruple::power_of_2, check_values[7]>::check();
        validator2_t<&quadruple::power_of_2, check_values[8]>::check();
        validator2_t<&quadruple::power_of_2, check_values[9]>::check();
        validator2_t<&quadruple::power_of_2, check_values[10]>::check();
        validator2_t<&quadruple::power_of_2, check_values[11]>::check();
        validator2_t<&quadruple::power_of_2, check_values[12]>::check();
        validator2_t<&quadruple::power_of_2, check_values[13]>::check();
        validator2_t<&quadruple::power_of_2, check_values[14]>::check();
        validator2_t<&quadruple::power_of_2, check_values[15]>::check();
    }
}
