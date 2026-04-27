#include <catch2/catch_all.hpp>
#include <cfenv>

#include "quadruple.hpp"
#include "test_helpers/test_utils.hpp"
#include "utils.hpp"

#include <cstring>
#include <numbers>

TEMPLATE_TEST_CASE("unary operators", "[arithmetics]", float, double) {
    auto check_plus = [](TestType val) {
        quadruple val_converted_1 = quadruple{+val};
        quadruple val_converted_2 = +quadruple{val};
        // compare bits
        REQUIRE(std::memcmp(&val_converted_1, &val_converted_2, sizeof(quadruple)) == 0);
    };
    auto check_minus = [](TestType val) {
        quadruple val_converted_1 = quadruple{-val};
        quadruple val_converted_2 = -quadruple{val};
        // compare bits
        REQUIRE(std::memcmp(&val_converted_1, &val_converted_2, sizeof(quadruple)) == 0);
    };

    SECTION("positive") {
        SECTION("constants") {
            SECTION("pi") {
                check_plus(std::numbers::pi_v<TestType>);
                check_minus(std::numbers::pi_v<TestType>);
            }
            SECTION("e") {
                check_plus(std::numbers::e_v<TestType>);
                check_minus(std::numbers::e_v<TestType>);
            }
            SECTION("phi") {
                check_plus(std::numbers::phi_v<TestType>);
                check_minus(std::numbers::phi_v<TestType>);
            }
            SECTION("sqrt2") {
                check_plus(std::numbers::sqrt2_v<TestType>);
                check_minus(std::numbers::sqrt2_v<TestType>);
            }
            SECTION("sqrt3") {
                check_plus(std::numbers::sqrt3_v<TestType>);
                check_minus(std::numbers::sqrt3_v<TestType>);
            }
            SECTION("inv_sqrtpi") {
                check_plus(std::numbers::inv_sqrtpi_v<TestType>);
                check_minus(std::numbers::inv_sqrtpi_v<TestType>);
            }
            SECTION("infinity") {
                check_plus(std::numeric_limits<TestType>::infinity());
                check_minus(std::numeric_limits<TestType>::infinity());
            }
            SECTION("qNaN") {
                check_plus(std::numeric_limits<TestType>::quiet_NaN());
                check_minus(std::numeric_limits<TestType>::quiet_NaN());
            }
            SECTION("sNaN") {
                check_plus(std::numeric_limits<TestType>::signaling_NaN());
                check_minus(std::numeric_limits<TestType>::signaling_NaN());
            }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_plus(value);
                check_minus(value);
            }
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_plus(value);
                check_minus(value);
            }
        }
    }
    SECTION("negative") {
        SECTION("constants") {
            SECTION("pi") {
                check_plus(-std::numbers::pi_v<TestType>);
                check_minus(-std::numbers::pi_v<TestType>);
            }
            SECTION("e") {
                check_plus(-std::numbers::e_v<TestType>);
                check_minus(-std::numbers::e_v<TestType>);
            }
            SECTION("phi") {
                check_plus(-std::numbers::phi_v<TestType>);
                check_minus(-std::numbers::phi_v<TestType>);
            }
            SECTION("sqrt2") {
                check_plus(-std::numbers::sqrt2_v<TestType>);
                check_minus(-std::numbers::sqrt2_v<TestType>);
            }
            SECTION("sqrt3") {
                check_plus(-std::numbers::sqrt3_v<TestType>);
                check_minus(-std::numbers::sqrt3_v<TestType>);
            }
            SECTION("inv_sqrtpi") {
                check_plus(-std::numbers::inv_sqrtpi_v<TestType>);
                check_minus(-std::numbers::inv_sqrtpi_v<TestType>);
            }
            SECTION("infinity") {
                check_plus(-std::numeric_limits<TestType>::infinity());
                check_minus(-std::numeric_limits<TestType>::infinity());
            }
            SECTION("qNaN") {
                check_plus(-std::numeric_limits<TestType>::quiet_NaN());
                check_minus(-std::numeric_limits<TestType>::quiet_NaN());
            }
            SECTION("sNaN") {
                check_plus(negative_sNaN<TestType>());
                check_minus(negative_sNaN<TestType>());
            }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_plus(-value);
                check_minus(-value);
            }
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_plus(-value);
                check_minus(-value);
            }
        }
    }
}

TEMPLATE_TEST_CASE("addition", "[arithmetics]", float, double) {
    auto check_addition = [](TestType val) {
        TestType result = val + val;
        quadruple val_converted{val};
        std::feclearexcept(FE_ALL_EXCEPT);
        TestType converted_back{val_converted + val_converted};
        if (is_sNaN(val)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_back));
            REQUIRE_FALSE(is_sNaN(converted_back));
        } else if (std::isnan(val)) {
            REQUIRE(std::isnan(converted_back));
        } else {
            // compare bits
            REQUIRE(std::memcmp(&result, &converted_back, sizeof(TestType)) == 0);
        }
    };
    auto check_addition_opposites = [](TestType val) {
        TestType result = val + (-val);
        quadruple val_converted{val};
        std::feclearexcept(FE_ALL_EXCEPT);
        TestType converted_back{val_converted + (-val_converted)};
        if (is_sNaN(val)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_back));
            REQUIRE_FALSE(is_sNaN(converted_back));
        } else if (std::isnan(val)) {
            REQUIRE(std::isnan(converted_back));
        } else {
            // compare bits
            REQUIRE(std::memcmp(&result, &converted_back, sizeof(TestType)) == 0);
        }
    };

    SECTION("positive") {
        SECTION("constants") {
            SECTION("pi") { check_addition(std::numbers::pi_v<TestType>); }
            SECTION("e") { check_addition(std::numbers::e_v<TestType>); }
            SECTION("phi") { check_addition(std::numbers::phi_v<TestType>); }
            SECTION("sqrt2") { check_addition(std::numbers::sqrt2_v<TestType>); }
            SECTION("sqrt3") { check_addition(std::numbers::sqrt3_v<TestType>); }
            SECTION("inv_sqrtpi") { check_addition(std::numbers::inv_sqrtpi_v<TestType>); }
            SECTION("infinity") { check_addition(std::numeric_limits<TestType>::infinity()); }
            SECTION("qNaN") { check_addition(std::numeric_limits<TestType>::quiet_NaN()); }
            SECTION("sNaN") { check_addition(std::numeric_limits<TestType>::signaling_NaN()); }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            remove_NaNs(generated);
            for (auto value : generated) {
                check_addition(value);
            }
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_addition(value);
            }
        }
    }

    SECTION("negative") {
        SECTION("constants") {
            SECTION("pi") { check_addition(-std::numbers::pi_v<TestType>); }
            SECTION("e") { check_addition(-std::numbers::e_v<TestType>); }
            SECTION("phi") { check_addition(-std::numbers::phi_v<TestType>); }
            SECTION("sqrt2") { check_addition(-std::numbers::sqrt2_v<TestType>); }
            SECTION("sqrt3") { check_addition(-std::numbers::sqrt3_v<TestType>); }
            SECTION("inv_sqrtpi") { check_addition(-std::numbers::inv_sqrtpi_v<TestType>); }
            SECTION("infinity") { check_addition(-std::numeric_limits<TestType>::infinity()); }
            SECTION("sNaN") { check_addition(-std::numeric_limits<TestType>::signaling_NaN()); }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            remove_NaNs(generated);
            for (auto value : generated) {
                check_addition(-value);
            }
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_addition(-value);
            }
        }
    }

    SECTION("opposites") {
        SECTION("constants") {
            SECTION("pi") {
                check_addition_opposites(std::numbers::pi_v<TestType>);
                check_addition_opposites(-std::numbers::pi_v<TestType>);
            }
            SECTION("e") {
                check_addition_opposites(std::numbers::e_v<TestType>);
                check_addition_opposites(-std::numbers::e_v<TestType>);
            }
            SECTION("phi") {
                check_addition_opposites(std::numbers::phi_v<TestType>);
                check_addition_opposites(-std::numbers::phi_v<TestType>);
            }
            SECTION("sqrt2") {
                check_addition_opposites(std::numbers::sqrt2_v<TestType>);
                check_addition_opposites(-std::numbers::sqrt2_v<TestType>);
            }
            SECTION("sqrt3") {
                check_addition_opposites(std::numbers::sqrt3_v<TestType>);
                check_addition_opposites(-std::numbers::sqrt3_v<TestType>);
            }
            SECTION("inv_sqrtpi") {
                check_addition_opposites(std::numbers::inv_sqrtpi_v<TestType>);
                check_addition_opposites(-std::numbers::inv_sqrtpi_v<TestType>);
            }
            SECTION("infinity") {
                check_addition_opposites(std::numeric_limits<TestType>::infinity());
                check_addition_opposites(-std::numeric_limits<TestType>::infinity());
            }
            SECTION("qNaN") {
                check_addition_opposites(std::numeric_limits<TestType>::quiet_NaN());
                check_addition_opposites(-std::numeric_limits<TestType>::quiet_NaN());
            }
            SECTION("sNaN") {
                check_addition_opposites(std::numeric_limits<TestType>::signaling_NaN());
                check_addition_opposites(negative_sNaN<TestType>());
            }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            remove_NaNs(generated);
            for (auto value : generated) {
                check_addition_opposites(value);
                check_addition_opposites(-value);
            }
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            for (auto value : generated) {
                check_addition_opposites(value);
                check_addition_opposites(-value);
            }
        }
    }

    SECTION("signed zero") {
        SECTION("+/+") { REQUIRE(std::signbit(TestType{} + TestType{}) == (quadruple{} + quadruple{}).signbit()); }
        SECTION("+/-") {
            REQUIRE(std::signbit(TestType{} + (-TestType{})) == (quadruple{} + (-quadruple{})).signbit());
        }
        SECTION("-/+") {
            REQUIRE(std::signbit((-TestType{}) + TestType{}) == ((-quadruple{}) + quadruple{}).signbit());
        }
        SECTION("-/-") {
            REQUIRE(std::signbit((-TestType{}) + (-TestType{})) == ((-quadruple{}) + (-quadruple{})).signbit());
        }
    }
}

TEMPLATE_TEST_CASE("subtraction", "[arithmetics]", float, double) {
    auto check_subtraction_same = [](TestType val) {
        TestType val2 = is_sNaN(val) ? val : val + val;
        TestType result_val = is_sNaN(val) ? val : val2 - val;
        quadruple val_converted{val};
        quadruple val2_converted{val2};
        std::feclearexcept(FE_ALL_EXCEPT);
        TestType converted_back{val2_converted - val_converted};
        if (is_sNaN(val)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_back));
            REQUIRE_FALSE(is_sNaN(converted_back));
        } else if (std::isnan(val)) {
            REQUIRE(std::isnan(converted_back));
        } else {
            // compare bits
            REQUIRE(std::memcmp(&result_val, &converted_back, sizeof(TestType)) == 0);
        }
    };
    auto check_subtraction_different = [](TestType val) {
        TestType val2 = is_sNaN(val) ? val : val + val;
        TestType result_val = is_sNaN(val) ? val : val2 - val;
        quadruple val_converted{val};
        quadruple val2_converted{val2};
        std::feclearexcept(FE_ALL_EXCEPT);
        TestType converted_back{val2_converted - val_converted};
        if (is_sNaN(val)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_back));
            REQUIRE_FALSE(is_sNaN(converted_back));
        } else if (std::isnan(val)) {
            REQUIRE(std::isnan(converted_back));
        } else {
            // compare bits
            REQUIRE(std::memcmp(&result_val, &converted_back, sizeof(TestType)) == 0);
        }
    };

    SECTION("keep sign") {
        SECTION("positive") {
            SECTION("constants") {
                SECTION("pi") { check_subtraction_same(std::numbers::pi_v<TestType>); }
                SECTION("e") { check_subtraction_same(std::numbers::e_v<TestType>); }
                SECTION("phi") { check_subtraction_same(std::numbers::phi_v<TestType>); }
                SECTION("sqrt2") { check_subtraction_same(std::numbers::sqrt2_v<TestType>); }
                SECTION("sqrt3") { check_subtraction_same(std::numbers::sqrt3_v<TestType>); }
                SECTION("inv_sqrtpi") { check_subtraction_same(std::numbers::inv_sqrtpi_v<TestType>); }
                SECTION("infinity") { check_subtraction_same(std::numeric_limits<TestType>::infinity()); }
                SECTION("qNaN") { check_subtraction_same(std::numeric_limits<TestType>::quiet_NaN()); }
                SECTION("sNaN") { check_subtraction_same(std::numeric_limits<TestType>::signaling_NaN()); }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_same(value);
                }
            }
            SECTION("random subnormal numbers") {
                auto generated = generate_subnormal_numbers<TestType>(test_size);
                for (auto value : generated) {
                    check_subtraction_same(value);
                }
            }
        }

        SECTION("negative") {
            SECTION("constants") {
                SECTION("pi") { check_subtraction_same(-std::numbers::pi_v<TestType>); }
                SECTION("e") { check_subtraction_same(-std::numbers::e_v<TestType>); }
                SECTION("phi") { check_subtraction_same(-std::numbers::phi_v<TestType>); }
                SECTION("sqrt2") { check_subtraction_same(-std::numbers::sqrt2_v<TestType>); }
                SECTION("sqrt3") { check_subtraction_same(-std::numbers::sqrt3_v<TestType>); }
                SECTION("inv_sqrtpi") { check_subtraction_same(-std::numbers::inv_sqrtpi_v<TestType>); }
                SECTION("infinity") { check_subtraction_same(-std::numeric_limits<TestType>::infinity()); }
                SECTION("qNaN") { check_subtraction_same(-std::numeric_limits<TestType>::quiet_NaN()); }
                SECTION("sNaN") { check_subtraction_same(negative_sNaN<TestType>()); }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_same(-value);
                }
            }
            SECTION("random subnormal numbers") {
                auto generated = generate_subnormal_numbers<TestType>(test_size);
                for (auto value : generated) {
                    check_subtraction_same(-value);
                }
            }
        }
    }

    SECTION("change sign") {
        SECTION("positive") {
            SECTION("constants") {
                SECTION("pi") { check_subtraction_different(std::numbers::pi_v<TestType>); }
                SECTION("e") { check_subtraction_different(std::numbers::e_v<TestType>); }
                SECTION("phi") { check_subtraction_different(std::numbers::phi_v<TestType>); }
                SECTION("sqrt2") { check_subtraction_different(std::numbers::sqrt2_v<TestType>); }
                SECTION("sqrt3") { check_subtraction_different(std::numbers::sqrt3_v<TestType>); }
                SECTION("inv_sqrtpi") { check_subtraction_different(std::numbers::inv_sqrtpi_v<TestType>); }
                SECTION("infinity") { check_subtraction_different(std::numeric_limits<TestType>::infinity()); }
                SECTION("qNaN") { check_subtraction_different(std::numeric_limits<TestType>::quiet_NaN()); }
                SECTION("sNaN") { check_subtraction_different(std::numeric_limits<TestType>::signaling_NaN()); }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_different(value);
                }
            }
            SECTION("random subnormal numbers") {
                auto generated = generate_subnormal_numbers<TestType>(test_size);
                for (auto value : generated) {
                    check_subtraction_different(value);
                }
            }
        }
        SECTION("negative") {
            SECTION("constants") {
                SECTION("pi") { check_subtraction_different(-std::numbers::pi_v<TestType>); }
                SECTION("e") { check_subtraction_different(-std::numbers::e_v<TestType>); }
                SECTION("phi") { check_subtraction_different(-std::numbers::phi_v<TestType>); }
                SECTION("sqrt2") { check_subtraction_different(-std::numbers::sqrt2_v<TestType>); }
                SECTION("sqrt3") { check_subtraction_different(-std::numbers::sqrt3_v<TestType>); }
                SECTION("inv_sqrtpi") { check_subtraction_different(-std::numbers::inv_sqrtpi_v<TestType>); }
                SECTION("infinity") { check_subtraction_different(-std::numeric_limits<TestType>::infinity()); }
                SECTION("qNaN") { check_subtraction_different(-std::numeric_limits<TestType>::quiet_NaN()); }
                SECTION("sNaN") { check_subtraction_different(negative_sNaN<TestType>()); }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_different(-value);
                }
            }
            SECTION("random subnormal numbers") {
                auto generated = generate_subnormal_numbers<TestType>(test_size);
                for (auto value : generated) {
                    check_subtraction_different(-value);
                }
            }
        }
    }

    SECTION("signed zero") {
        SECTION("+/+") { REQUIRE(std::signbit(TestType{} - TestType{}) == (quadruple{} - quadruple{}).signbit()); }
        SECTION("+/-") {
            REQUIRE(std::signbit(TestType{} - (-TestType{})) == (quadruple{} - (-quadruple{})).signbit());
        }
        SECTION("-/+") {
            REQUIRE(std::signbit((-TestType{}) - TestType{}) == ((-quadruple{}) - quadruple{}).signbit());
        }
        SECTION("-/-") {
            REQUIRE(std::signbit((-TestType{}) - (-TestType{})) == ((-quadruple{}) - (-quadruple{})).signbit());
        }
    }
}

// TODO: test quadruple subnormals
TEMPLATE_TEST_CASE("multiplication", "[arithmetics]", float, double) {
    auto check_multiplication = [](TestType val1, TestType val2) {
        // both float and double multiplication will round up
        // and rounding supposed to be the same as from quadruple to double
        quadruple converted_val1{val1};
        quadruple converted_val2{val2};
        TestType result = val1 * val2;
        std::feclearexcept(FE_ALL_EXCEPT);
        quadruple test_result = converted_val1 * converted_val2;
        TestType converted_test_result{test_result};
        if (is_sNaN(val1) || is_sNaN(val2)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_test_result));
            REQUIRE_FALSE(is_sNaN(converted_test_result));
        } else {
            // compare bits
            REQUIRE(std::memcmp(&result, &converted_test_result, sizeof(TestType)) == 0);
        }
    };

    SECTION("positive") {
        SECTION("constants") {
            std::vector<TestType> test_constants;

            test_constants.emplace_back(TestType{0});
            test_constants.emplace_back(std::numbers::pi_v<TestType>);
            test_constants.emplace_back(std::numbers::e_v<TestType>);
            test_constants.emplace_back(std::numbers::phi_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt2_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt3_v<TestType>);
            test_constants.emplace_back(std::numbers::inv_sqrtpi_v<TestType>);
            test_constants.emplace_back(std::numeric_limits<TestType>::infinity());
            test_constants.emplace_back(std::numeric_limits<TestType>::quiet_NaN());
            test_constants.emplace_back(std::numeric_limits<TestType>::signaling_NaN());

            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_multiplication(test_constants.at(i), test_constants.at(j));
                }
            }
        }
        SECTION("random numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random normal*subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated_n = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            auto generated_s = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated_n);

            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_multiplication(generated_n.at(i), generated_s.at(j));
                }
            }

            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_multiplication(generated_s.at(i), generated_n.at(j));
                }
            }
        }
    }

    SECTION("negative") {
        SECTION("constants") {
            std::vector<TestType> test_constants;

            test_constants.emplace_back(-TestType{0});
            test_constants.emplace_back(-std::numbers::pi_v<TestType>);
            test_constants.emplace_back(-std::numbers::e_v<TestType>);
            test_constants.emplace_back(-std::numbers::phi_v<TestType>);
            test_constants.emplace_back(-std::numbers::sqrt2_v<TestType>);
            test_constants.emplace_back(-std::numbers::sqrt3_v<TestType>);
            test_constants.emplace_back(-std::numbers::inv_sqrtpi_v<TestType>);
            test_constants.emplace_back(-std::numeric_limits<TestType>::infinity());
            test_constants.emplace_back(-std::numeric_limits<TestType>::quiet_NaN());
            test_constants.emplace_back(-std::numeric_limits<TestType>::signaling_NaN());

            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_multiplication(test_constants.at(i), test_constants.at(j));
                }
            }
        }
        SECTION("random numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(-generated.at(i), -generated.at(j));
                }
            }
        }
        SECTION("random subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(-generated.at(i), -generated.at(j));
                }
            }
        }
        SECTION("random normal*subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated_n = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            auto generated_s = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated_n);

            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_multiplication(-generated_n.at(i), -generated_s.at(j));
                }
            }

            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_multiplication(-generated_s.at(i), -generated_n.at(j));
                }
            }
        }
    }

    SECTION("opposites") {
        SECTION("constants") {
            std::vector<TestType> test_constants;

            test_constants.emplace_back(TestType{0});
            test_constants.emplace_back(std::numbers::pi_v<TestType>);
            test_constants.emplace_back(std::numbers::e_v<TestType>);
            test_constants.emplace_back(std::numbers::phi_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt2_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt3_v<TestType>);
            test_constants.emplace_back(std::numbers::inv_sqrtpi_v<TestType>);
            test_constants.emplace_back(std::numeric_limits<TestType>::infinity());
            test_constants.emplace_back(std::numeric_limits<TestType>::quiet_NaN());
            test_constants.emplace_back(std::numeric_limits<TestType>::signaling_NaN());

            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_multiplication(test_constants.at(i), -test_constants.at(j));
                }
            }
            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_multiplication(-test_constants.at(i), test_constants.at(j));
                }
            }
        }
        SECTION("random numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(generated.at(i), -generated.at(j));
                }
            }
            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(-generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(generated.at(i), -generated.at(j));
                }
            }
            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_multiplication(-generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random normal*subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated_n = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            auto generated_s = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated_n);

            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_multiplication(generated_n.at(i), -generated_s.at(j));
                }
            }
            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_multiplication(-generated_n.at(i), generated_s.at(j));
                }
            }

            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_multiplication(generated_s.at(i), -generated_n.at(j));
                }
            }
            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_multiplication(-generated_s.at(i), generated_n.at(j));
                }
            }
        }
    }

    SECTION("signed zero") {
        SECTION("+/+") { REQUIRE(std::signbit(TestType{} * TestType{}) == (quadruple{} * quadruple{}).signbit()); }
        SECTION("+/-") {
            REQUIRE(std::signbit(TestType{} * (-TestType{})) == (quadruple{} * (-quadruple{})).signbit());
        }
        SECTION("-/+") {
            REQUIRE(std::signbit((-TestType{}) * TestType{}) == ((-quadruple{}) * quadruple{}).signbit());
        }
        SECTION("-/-") {
            REQUIRE(std::signbit((-TestType{}) * (-TestType{})) == ((-quadruple{}) * (-quadruple{})).signbit());
        }
    }
}

TEMPLATE_TEST_CASE("division", "[arithmetics]", float, double) {
    auto check_division = [](TestType val1, TestType val2) {
        // both float and double divisions will round down
        // and rounding supposed to be the same as from quadruple to double
        quadruple converted_val1{val1};
        quadruple converted_val2{val2};
        TestType result = val1 / val2;
        std::feclearexcept(FE_ALL_EXCEPT);
        quadruple test_result = converted_val1 / converted_val2;
        TestType converted_test_result{test_result};
        if (is_sNaN(val1) || is_sNaN(val2)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_test_result));
            REQUIRE_FALSE(is_sNaN(converted_test_result));
        } else if (std::isnan(result)) {
            REQUIRE(std::isnan(converted_test_result));
        } else {
            // compare bits
            REQUIRE(std::memcmp(&result, &converted_test_result, sizeof(TestType)) == 0);
        }
    };

    SECTION("positive") {
        SECTION("constants") {
            std::vector<TestType> test_constants;

            test_constants.emplace_back(TestType{0});
            test_constants.emplace_back(TestType{4});
            test_constants.emplace_back(std::numbers::pi_v<TestType>);
            test_constants.emplace_back(std::numbers::e_v<TestType>);
            test_constants.emplace_back(std::numbers::phi_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt2_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt3_v<TestType>);
            test_constants.emplace_back(std::numbers::inv_sqrtpi_v<TestType>);
            test_constants.emplace_back(std::numeric_limits<TestType>::infinity());
            test_constants.emplace_back(std::numeric_limits<TestType>::quiet_NaN());
            test_constants.emplace_back(std::numeric_limits<TestType>::signaling_NaN());

            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_division(test_constants.at(i), test_constants.at(j));
                }
            }
        }
        SECTION("random numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random normal / subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated_n = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            auto generated_s = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated_n);

            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_division(generated_n.at(i), generated_s.at(j));
                }
            }

            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_division(generated_s.at(i), generated_n.at(j));
                }
            }
        }
    }

    SECTION("negative") {
        SECTION("constants") {
            std::vector<TestType> test_constants;

            test_constants.emplace_back(-TestType{0});
            test_constants.emplace_back(-TestType{4});
            test_constants.emplace_back(-std::numbers::pi_v<TestType>);
            test_constants.emplace_back(-std::numbers::e_v<TestType>);
            test_constants.emplace_back(-std::numbers::phi_v<TestType>);
            test_constants.emplace_back(-std::numbers::sqrt2_v<TestType>);
            test_constants.emplace_back(-std::numbers::sqrt3_v<TestType>);
            test_constants.emplace_back(-std::numbers::inv_sqrtpi_v<TestType>);
            test_constants.emplace_back(-std::numeric_limits<TestType>::infinity());
            test_constants.emplace_back(-std::numeric_limits<TestType>::quiet_NaN());
            test_constants.emplace_back(-std::numeric_limits<TestType>::signaling_NaN());

            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_division(test_constants.at(i), test_constants.at(j));
                }
            }
        }
        SECTION("random numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(-generated.at(i), -generated.at(j));
                }
            }
        }
        SECTION("random subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(-generated.at(i), -generated.at(j));
                }
            }
        }
        SECTION("random normal*subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated_n = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            auto generated_s = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated_n);

            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_division(-generated_n.at(i), -generated_s.at(j));
                }
            }

            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_division(-generated_s.at(i), -generated_n.at(j));
                }
            }
        }
    }

    SECTION("opposites") {
        SECTION("constants") {
            std::vector<TestType> test_constants;

            test_constants.emplace_back(TestType{0});
            test_constants.emplace_back(TestType{4});
            test_constants.emplace_back(std::numbers::pi_v<TestType>);
            test_constants.emplace_back(std::numbers::e_v<TestType>);
            test_constants.emplace_back(std::numbers::phi_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt2_v<TestType>);
            test_constants.emplace_back(std::numbers::sqrt3_v<TestType>);
            test_constants.emplace_back(std::numbers::inv_sqrtpi_v<TestType>);
            test_constants.emplace_back(std::numeric_limits<TestType>::infinity());
            test_constants.emplace_back(std::numeric_limits<TestType>::quiet_NaN());
            test_constants.emplace_back(std::numeric_limits<TestType>::signaling_NaN());

            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_division(test_constants.at(i), -test_constants.at(j));
                }
            }
            for (size_t i = 0; i < test_constants.size(); i++) {
                for (size_t j = 0; j < test_constants.size(); j++) {
                    check_division(-test_constants.at(i), test_constants.at(j));
                }
            }
        }
        SECTION("random numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(generated.at(i), -generated.at(j));
                }
            }
            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(-generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated);

            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(generated.at(i), -generated.at(j));
                }
            }
            for (size_t i = 0; i < generated.size(); i++) {
                for (size_t j = 0; j < generated.size(); j++) {
                    check_division(-generated.at(i), generated.at(j));
                }
            }
        }
        SECTION("random normal*subnormal numbers") {
            // sqrt for test size, otherwise it will take way too long to run
            auto generated_n = generate_normal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            auto generated_s = generate_subnormal_numbers<TestType>(static_cast<size_t>(sqrt(test_size)));
            remove_NaNs(generated_n);

            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_division(generated_n.at(i), -generated_s.at(j));
                }
            }
            for (size_t i = 0; i < generated_n.size(); i++) {
                for (size_t j = 0; j < generated_s.size(); j++) {
                    check_division(-generated_n.at(i), generated_s.at(j));
                }
            }

            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_division(generated_s.at(i), -generated_n.at(j));
                }
            }
            for (size_t i = 0; i < generated_s.size(); i++) {
                for (size_t j = 0; j < generated_n.size(); j++) {
                    check_division(-generated_s.at(i), generated_n.at(j));
                }
            }
        }
    }

    SECTION("signed zero") {
        SECTION("+/+") { REQUIRE(std::signbit(TestType{} / TestType{}) == (quadruple{} / quadruple{}).signbit()); }
        SECTION("+/-") {
            REQUIRE(std::signbit(TestType{} / (-TestType{})) == (quadruple{} / (-quadruple{})).signbit());
        }
        SECTION("-/+") {
            REQUIRE(std::signbit((-TestType{}) / TestType{}) == ((-quadruple{}) / quadruple{}).signbit());
        }
        SECTION("-/-") {
            REQUIRE(std::signbit((-TestType{}) / (-TestType{})) == ((-quadruple{}) / (-quadruple{})).signbit());
        }
    }
}

TEST_CASE("quadruple subnormals", "[arithmetics]") {
    SECTION("subnormal to normal") {
        quadruple sub_normal1{};
        quadruple sub_normal2{};

        // equivalent to 0x0000FFFFFFFFFFFF FFFFFFFFFFFFFFFF
        std::memset(reinterpret_cast<char*>(&sub_normal1), 255, 14);
        // equivalent to 0x0000000000000000 0000000000000001
        std::memset(reinterpret_cast<char*>(&sub_normal2), 1, 1);

        quadruple res = sub_normal1 + sub_normal2;
        REQUIRE(!res.is_subnormal());
        REQUIRE(res == quadruple::min());
    }
    SECTION("normal to subnormal") {
        quadruple normal = quadruple::min();
        quadruple sub_normal{};

        SECTION("too small to affect") {
            // equivalent to 0x0000000000000000 0000000000000001
            std::memset(reinterpret_cast<char*>(&sub_normal), 1, 1);

            quadruple res = normal - sub_normal;
            REQUIRE(!res.is_subnormal());
            REQUIRE(res == normal);
        }
        SECTION("proper subnormal") {
            // equivalent to 0x0000000000000000 0000000000000002
            std::memset(reinterpret_cast<char*>(&sub_normal), 2, 1);

            quadruple res = normal - sub_normal;
            REQUIRE(res.is_subnormal());
            REQUIRE(res < quadruple::min());
        }
    }
    SECTION("subnormal to subnormal") {
        quadruple sub_normal1{};
        quadruple sub_normal2{};

        // equivalent to 0x0000FFFFFFFFFFFF FFFFFFFFFFFFFFFF
        std::memset(reinterpret_cast<char*>(&sub_normal1), 255, 14);
        // equivalent to 0x0000000000000000 0000000000000001
        std::memset(reinterpret_cast<char*>(&sub_normal2), 1, 1);

        quadruple res = sub_normal1 - sub_normal2;
        REQUIRE(res.is_subnormal());
        REQUIRE(res < sub_normal1);

        res += sub_normal2;
        REQUIRE(res.is_subnormal());
        REQUIRE(res == sub_normal1);
    }
}

TEST_CASE("quadruple powers of 2", "[arithmetics]") {
    // +0
    REQUIRE(quadruple::power_of_2(quadruple_min_representable_pow2 - 1).is_zero());
    REQUIRE_FALSE(quadruple::power_of_2(quadruple_min_representable_pow2 - 1).signbit());

    SECTION("positive") {
        SECTION("addition") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = quadruple::power_of_2(exponent);
                auto summed = first_power + first_power;
                auto next_power = quadruple::power_of_2(exponent + 1);
                REQUIRE(summed == next_power);
            }
        }
        SECTION("subtraction") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = quadruple::power_of_2(exponent);
                auto next_power = quadruple::power_of_2(exponent + 1);
                auto subtracted = next_power - first_power;
                REQUIRE(subtracted == first_power);
            }
        }
        SECTION("multiplication") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = quadruple::power_of_2(exponent);
                auto multiplied = first_power * quadruple{2};
                auto next_power = quadruple::power_of_2(exponent + 1);
                REQUIRE(multiplied == next_power);
            }
        }
        SECTION("division") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = quadruple::power_of_2(exponent);
                auto next_power = quadruple::power_of_2(exponent + 1);
                auto divided = next_power / quadruple{2};
                REQUIRE(divided == first_power);
            }
        }
    }
    SECTION("negative") {
        SECTION("addition") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = -quadruple::power_of_2(exponent);
                auto summed = first_power + first_power;
                auto next_power = -quadruple::power_of_2(exponent + 1);
                REQUIRE(summed == next_power);
            }
        }
        SECTION("subtraction") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = -quadruple::power_of_2(exponent);
                auto next_power = -quadruple::power_of_2(exponent + 1);
                auto subtracted = next_power - first_power;
                REQUIRE(subtracted == first_power);
            }
        }
        SECTION("multiplication") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = -quadruple::power_of_2(exponent);
                auto multiplied = first_power * quadruple{2};
                auto next_power = -quadruple::power_of_2(exponent + 1);
                REQUIRE(multiplied == next_power);
            }
        }
        SECTION("division") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = -quadruple::power_of_2(exponent);
                auto next_power = -quadruple::power_of_2(exponent + 1);
                auto divided = next_power / quadruple{2};
                REQUIRE(divided == first_power);
            }
        }
    }

    REQUIRE(quadruple::power_of_2(quadruple_max_representable_pow2) * quadruple{2} == quadruple::infinity());
}
