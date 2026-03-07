#include <cfenv>
#include <catch2/catch_all.hpp>

#include "quadruple.hpp"
#include "utils.hpp"
#include "test_utils.hpp"

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
            SECTION("pi") {
                check_addition(std::numbers::pi_v<TestType>);
            }
            SECTION("e") {
                check_addition(std::numbers::e_v<TestType>);
            }
            SECTION("phi") {
                check_addition(std::numbers::phi_v<TestType>);
            }
            SECTION("sqrt2") {
                check_addition(std::numbers::sqrt2_v<TestType>);
            }
            SECTION("sqrt3") {
                check_addition(std::numbers::sqrt3_v<TestType>);
            }
            SECTION("inv_sqrtpi") {
                check_addition(std::numbers::inv_sqrtpi_v<TestType>);
            }
            SECTION("infinity") {
                check_addition(std::numeric_limits<TestType>::infinity());
            }
            SECTION("qNaN") {
                check_addition(std::numeric_limits<TestType>::quiet_NaN());
            }
            SECTION("sNaN") {
                check_addition(std::numeric_limits<TestType>::signaling_NaN());
            }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            remove_NaNs(generated);
            for (auto value : generated) {
                check_addition(value);
            }
        }
    }

    SECTION("negative") {
        SECTION("constants") {
            SECTION("pi") {
                check_addition(-std::numbers::pi_v<TestType>);
            }
            SECTION("e") {
                check_addition(-std::numbers::e_v<TestType>);
            }
            SECTION("phi") {
                check_addition(-std::numbers::phi_v<TestType>);
            }
            SECTION("sqrt2") {
                check_addition(-std::numbers::sqrt2_v<TestType>);
            }
            SECTION("sqrt3") {
                check_addition(-std::numbers::sqrt3_v<TestType>);
            }
            SECTION("inv_sqrtpi") {
                check_addition(-std::numbers::inv_sqrtpi_v<TestType>);
            }
            SECTION("infinity") {
                check_addition(-std::numeric_limits<TestType>::infinity());
            }
            SECTION("sNaN") {
                check_addition(-std::numeric_limits<TestType>::signaling_NaN());
            }
        }
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            remove_NaNs(generated);
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
    }

    SECTION("signed zero") {
        SECTION("+/+") {
            REQUIRE(std::signbit(TestType{} + TestType{}) == (quadruple{} + quadruple{}).signbit());
        }
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
                SECTION("pi") {
                    check_subtraction_same(std::numbers::pi_v<TestType>);
                }
                SECTION("e") {
                    check_subtraction_same(std::numbers::e_v<TestType>);
                }
                SECTION("phi") {
                    check_subtraction_same(std::numbers::phi_v<TestType>);
                }
                SECTION("sqrt2") {
                    check_subtraction_same(std::numbers::sqrt2_v<TestType>);
                }
                SECTION("sqrt3") {
                    check_subtraction_same(std::numbers::sqrt3_v<TestType>);
                }
                SECTION("inv_sqrtpi") {
                    check_subtraction_same(std::numbers::inv_sqrtpi_v<TestType>);
                }
                SECTION("infinity") {
                    check_subtraction_same(std::numeric_limits<TestType>::infinity());
                }
                SECTION("qNaN") {
                    check_subtraction_same(std::numeric_limits<TestType>::quiet_NaN());
                }
                SECTION("sNaN") {
                    check_subtraction_same(std::numeric_limits<TestType>::signaling_NaN());
                }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_same(value);
                }
            }
        }

        SECTION("negative") {
            SECTION("constants") {
                SECTION("pi") {
                    check_subtraction_same(-std::numbers::pi_v<TestType>);
                }
                SECTION("e") {
                    check_subtraction_same(-std::numbers::e_v<TestType>);
                }
                SECTION("phi") {
                    check_subtraction_same(-std::numbers::phi_v<TestType>);
                }
                SECTION("sqrt2") {
                    check_subtraction_same(-std::numbers::sqrt2_v<TestType>);
                }
                SECTION("sqrt3") {
                    check_subtraction_same(-std::numbers::sqrt3_v<TestType>);
                }
                SECTION("inv_sqrtpi") {
                    check_subtraction_same(-std::numbers::inv_sqrtpi_v<TestType>);
                }
                SECTION("infinity") {
                    check_subtraction_same(-std::numeric_limits<TestType>::infinity());
                }
                SECTION("qNaN") {
                    check_subtraction_same(-std::numeric_limits<TestType>::quiet_NaN());
                }
                SECTION("sNaN") {
                    check_subtraction_same(negative_sNaN<TestType>());
                }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_same(-value);
                }
            }
        }
    }

    SECTION("change sign") {
        SECTION("positive") {
            SECTION("constants") {
                SECTION("pi") {
                    check_subtraction_different(std::numbers::pi_v<TestType>);
                }
                SECTION("e") {
                    check_subtraction_different(std::numbers::e_v<TestType>);
                }
                SECTION("phi") {
                    check_subtraction_different(std::numbers::phi_v<TestType>);
                }
                SECTION("sqrt2") {
                    check_subtraction_different(std::numbers::sqrt2_v<TestType>);
                }
                SECTION("sqrt3") {
                    check_subtraction_different(std::numbers::sqrt3_v<TestType>);
                }
                SECTION("inv_sqrtpi") {
                    check_subtraction_different(std::numbers::inv_sqrtpi_v<TestType>);
                }
                SECTION("infinity") {
                    check_subtraction_different(std::numeric_limits<TestType>::infinity());
                }
                SECTION("qNaN") {
                    check_subtraction_different(std::numeric_limits<TestType>::quiet_NaN());
                }
                SECTION("sNaN") {
                    check_subtraction_different(std::numeric_limits<TestType>::signaling_NaN());
                }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_different(value);
                }
            }
        }
        SECTION("negative") {
            SECTION("constants") {
                SECTION("pi") {
                    check_subtraction_different(-std::numbers::pi_v<TestType>);
                }
                SECTION("e") {
                    check_subtraction_different(-std::numbers::e_v<TestType>);
                }
                SECTION("phi") {
                    check_subtraction_different(-std::numbers::phi_v<TestType>);
                }
                SECTION("sqrt2") {
                    check_subtraction_different(-std::numbers::sqrt2_v<TestType>);
                }
                SECTION("sqrt3") {
                    check_subtraction_different(-std::numbers::sqrt3_v<TestType>);
                }
                SECTION("inv_sqrtpi") {
                    check_subtraction_different(-std::numbers::inv_sqrtpi_v<TestType>);
                }
                SECTION("infinity") {
                    check_subtraction_different(-std::numeric_limits<TestType>::infinity());
                }
                SECTION("qNaN") {
                    check_subtraction_different(-std::numeric_limits<TestType>::quiet_NaN());
                }
                SECTION("sNaN") {
                    check_subtraction_different(negative_sNaN<TestType>());
                }
            }
            SECTION("random numbers") {
                auto generated = generate_normal_numbers<TestType>(test_size);
                remove_NaNs(generated);
                for (auto value : generated) {
                    check_subtraction_different(-value);
                }
            }
        }
    }

    SECTION("signed zero") {
        SECTION("+/+") {
            REQUIRE(std::signbit(TestType{} - TestType{}) == (quadruple{} - quadruple{}).signbit());
        }
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