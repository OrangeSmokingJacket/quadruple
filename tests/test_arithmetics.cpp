#include <catch2/catch_all.hpp>

#include "quadruple.hpp"
#include <cstring>
#include <numbers>

// There are some warnings emerged from Catch2 macros
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

TEMPLATE_TEST_CASE("addition", "[arithmetics]", float, double) {
    auto check_addition = [](TestType val) {
        TestType result = val + val;
        quadruple val_converted{val};
        TestType converted_back{val_converted + val_converted};
        // compare bits
        REQUIRE(std::memcmp(&result, &converted_back, sizeof(TestType)) == 0);
    };
    auto check_addition_opposites = [](TestType val) {
        TestType result = val + (-val);
        quadruple val_converted{val};
        TestType converted_back{val_converted + (-val_converted)};
        // compare bits
        REQUIRE(std::memcmp(&result, &converted_back, sizeof(TestType)) == 0);
    };

    SECTION("positive") {
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
    }
    SECTION("negative") {
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
        SECTION("qNaN") {
            check_addition(-std::numeric_limits<TestType>::quiet_NaN());
        }
    }
    SECTION("opposites") {
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
        TestType val2 = val + val;
        TestType result_val = val2 - val;
        quadruple val_converted{val};
        quadruple val2_converted{val2};
        TestType converted_back{val2_converted - val_converted};
        // compare bits
        REQUIRE(std::memcmp(&result_val, &converted_back, sizeof(TestType)) == 0);
    };
    auto check_subtraction_different = [](TestType val) {
        TestType val2 = val + val;
        TestType result_val = val2 - val;
        quadruple val_converted{val};
        quadruple val2_converted{val2};
        TestType converted_back{val2_converted - val_converted};
        // compare bits
        REQUIRE(std::memcmp(&result_val, &converted_back, sizeof(TestType)) == 0);
    };

    SECTION("keep sign") {
        SECTION("positive") {
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
        }
        SECTION("negative") {
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
        }
    }
    SECTION("change sign") {
        SECTION("positive") {
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
        }
        SECTION("negative") {
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

#pragma GCC diagnostic pop