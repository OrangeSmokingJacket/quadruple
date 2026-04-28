#include <catch2/catch_all.hpp>
#include <cfenv>

#include "quadruple.hpp"
#include "test_helpers/test_utils.hpp"
#include "utils.hpp"

#include <cstring>
#include <numbers>

// generate test values
template <typename T>
requires std::is_floating_point_v<T>
static constexpr std::array test_constants_pos{T{0},
                                               T{4},
                                               std::numbers::pi_v<T>,
                                               std::numbers::e_v<T>,
                                               std::numbers::phi_v<T>,
                                               std::numbers::sqrt2_v<T>,
                                               std::numbers::sqrt3_v<T>,
                                               std::numbers::inv_sqrtpi_v<T>,
                                               std::numeric_limits<T>::infinity(),
                                               std::numeric_limits<T>::quiet_NaN(),
                                               std::numeric_limits<T>::signaling_NaN()};

template <typename T>
requires std::is_floating_point_v<T>
constexpr auto test_constants_neg = []() {
    auto altered_copy = test_constants_pos<T>;
    std::for_each(altered_copy.begin(), altered_copy.end(), [](T& constant) { constant = -constant; });
    return altered_copy;
}();

// TODO: test quadruple subnormals

template <typename OperatorStruct>
constexpr bool CanOverflow = false;

template <>
constexpr bool CanOverflow<std::plus<>> = true;
template <>
constexpr bool CanOverflow<std::minus<>> = true;

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

    auto generate_normals = generate_normal_numbers<TestType>(test_size);
    auto generate_subnormals = generate_subnormal_numbers<TestType>(test_size);

    SECTION("positive") {
        SECTION("constants") {
            std::for_each(test_constants_pos<TestType>.begin(), test_constants_pos<TestType>.end(), check_plus);
            std::for_each(test_constants_pos<TestType>.begin(), test_constants_pos<TestType>.end(), check_minus);
        }
        SECTION("random numbers") {
            for (auto value : generate_normals) {
                check_plus(value);
                check_minus(value);
            }
        }
        SECTION("random subnormal numbers") {
            for (auto value : generate_subnormals) {
                check_plus(value);
                check_minus(value);
            }
        }
    }
    SECTION("negative") {
        SECTION("constants") {
            std::for_each(test_constants_neg<TestType>.begin(), test_constants_neg<TestType>.end(), check_plus);
            std::for_each(test_constants_neg<TestType>.begin(), test_constants_neg<TestType>.end(), check_minus);
        }
        SECTION("random numbers") {
            for (auto value : generate_normals) {
                check_plus(-value);
                check_minus(-value);
            }
        }
        SECTION("random subnormal numbers") {
            for (auto value : generate_subnormals) {
                check_plus(-value);
                check_minus(-value);
            }
        }
    }
}

TEMPLATE_TEST_CASE_SIG("basic operators",
                       "[arithmetics]",
                       ((typename ValueType, typename OperatorStruct), ValueType, OperatorStruct),
                       (float, std::plus<>),
                       (float, std::minus<>),
                       (float, std::multiplies<>),
                       (float, std::divides<>),
                       (double, std::plus<>),
                       (double, std::minus<>),
                       (double, std::multiplies<>),
                       (double, std::divides<>)) {
    auto check_operation = [call_operator = OperatorStruct{}](ValueType val1, ValueType val2) {
        quadruple converted_val1{val1};
        quadruple converted_val2{val2};
        ValueType result = call_operator(val1, val2);
        std::feclearexcept(FE_ALL_EXCEPT);
        quadruple test_result = call_operator(converted_val1, converted_val2);
        ValueType converted_test_result{test_result};
        if (is_sNaN(val1) || is_sNaN(val2)) {
            REQUIRE(std::fetestexcept(FE_INVALID));
            REQUIRE(std::isnan(converted_test_result));
            REQUIRE_FALSE(is_sNaN(converted_test_result));
        } else if (std::isnan(result)) {
            REQUIRE(std::isnan(converted_test_result));
        } else {
            if constexpr (CanOverflow<OperatorStruct>) {
                // due to rounding we could be one of the given target
                ValueType result_next = result < 0 ? std::nextafter(result, -std::numeric_limits<ValueType>::infinity())
                                                   : std::nextafter(result, std::numeric_limits<ValueType>::infinity());
                // compare bits
                bool equals_res = std::memcmp(&result, &converted_test_result, sizeof(ValueType)) == 0;
                bool equals_res_next = std::memcmp(&result_next, &converted_test_result, sizeof(ValueType)) == 0;
                REQUIRE((equals_res || equals_res_next));
            } else {
                // compare bits
                REQUIRE(std::memcmp(&result, &converted_test_result, sizeof(ValueType)) == 0);
            }
        }
    };

    // sqrt for test size, otherwise it will take way too long to run
    auto generated_normals_pos = generate_subnormal_numbers<ValueType>(static_cast<size_t>(sqrt(test_size)));
    auto generated_subnormal_pos = generate_subnormal_numbers<ValueType>(static_cast<size_t>(sqrt(test_size)));
    remove_NaNs(generated_normals_pos);
    // negated
    auto generated_normals_neg = generated_normals_pos;
    auto generated_subnormal_neg = generated_subnormal_pos;
    for (size_t i = 0; i < generated_normals_neg.size(); i++) {
        generated_normals_neg[i] = -generated_normals_neg[i];
    }
    for (size_t i = 0; i < generated_subnormal_neg.size(); i++) {
        generated_subnormal_neg[i] = -generated_subnormal_neg[i];
    }

    SECTION("positive") {
        SECTION("constants") {
            for_each_premutation(test_constants_pos<ValueType>, test_constants_pos<ValueType>, check_operation);
        }
        SECTION("random numbers") {
            for_each_premutation(generated_normals_pos, generated_normals_pos, check_operation);
        }
        SECTION("random subnormal numbers") {
            for_each_premutation(generated_subnormal_pos, generated_subnormal_pos, check_operation);
        }
        SECTION("random normal + subnormal numbers") {
            for_each_premutation(generated_normals_pos, generated_subnormal_pos, check_operation);
            for_each_premutation(generated_subnormal_pos, generated_normals_pos, check_operation);
        }
    }

    SECTION("negative") {
        SECTION("constants") {
            for_each_premutation(test_constants_neg<ValueType>, test_constants_neg<ValueType>, check_operation);
        }
        SECTION("random numbers") {
            for_each_premutation(generated_normals_neg, generated_normals_neg, check_operation);
        }
        SECTION("random subnormal numbers") {
            for_each_premutation(generated_subnormal_neg, generated_subnormal_neg, check_operation);
        }
        SECTION("random normal + subnormal numbers") {
            for_each_premutation(generated_normals_neg, generated_subnormal_neg, check_operation);
            for_each_premutation(generated_subnormal_neg, generated_normals_neg, check_operation);
        }
    }

    SECTION("opposites") {
        SECTION("constants") {
            for_each_premutation(test_constants_pos<ValueType>, test_constants_neg<ValueType>, check_operation);
            for_each_premutation(test_constants_neg<ValueType>, test_constants_pos<ValueType>, check_operation);
        }
        SECTION("random numbers") {
            for_each_premutation(generated_normals_pos, generated_normals_neg, check_operation);
            for_each_premutation(generated_normals_neg, generated_normals_pos, check_operation);
        }
        SECTION("random subnormal numbers") {
            for_each_premutation(generated_subnormal_pos, generated_subnormal_neg, check_operation);
            for_each_premutation(generated_subnormal_neg, generated_subnormal_pos, check_operation);
        }
        SECTION("random normal + subnormal numbers") {
            for_each_premutation(generated_normals_pos, generated_subnormal_neg, check_operation);
            for_each_premutation(generated_subnormal_pos, generated_normals_neg, check_operation);
            for_each_premutation(generated_normals_neg, generated_subnormal_pos, check_operation);
            for_each_premutation(generated_subnormal_neg, generated_normals_pos, check_operation);
        }
    }

    SECTION("signed zero") {
        OperatorStruct operator_call{};
        SECTION("+/+") {
            REQUIRE(std::signbit(operator_call(ValueType{}, ValueType{})) ==
                    operator_call(quadruple{}, quadruple{}).signbit());
        }
        SECTION("+/-") {
            REQUIRE(std::signbit(operator_call(ValueType{}, -ValueType{})) ==
                    operator_call(quadruple{}, -quadruple{}).signbit());
        }
        SECTION("-/+") {
            REQUIRE(std::signbit(operator_call(-ValueType{}, ValueType{})) ==
                    operator_call(-quadruple{}, quadruple{}).signbit());
        }
        SECTION("-/-") {
            // For some reason, this CAPTURE call fixes the result
            // in Release version only std::divides and operator/ produce different signed results
            if constexpr (std::is_same_v<OperatorStruct, std::divides<>>) {
                CAPTURE(std::signbit((-ValueType{}) / (-ValueType{})));
            }
            REQUIRE(std::signbit(operator_call(-ValueType{}, -ValueType{})) ==
                    operator_call(-quadruple{}, -quadruple{}).signbit());
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
    // anything less will be rounded to +0
    REQUIRE(quadruple::power_of_2(quadruple_min_representable_pow2 - 1).is_zero());
    REQUIRE_FALSE(quadruple::power_of_2(quadruple_min_representable_pow2 - 1).signbit());
    // anything greater will be rounded to +inf
    REQUIRE(quadruple::power_of_2(quadruple_max_representable_pow2) * quadruple{2} == quadruple::infinity());

    SECTION("positive") {
        SECTION("addition") {
            for (int32_t exponent = quadruple_min_representable_pow2; exponent <= quadruple_max_representable_pow2;
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
            for (int32_t exponent = quadruple_min_representable_pow2; exponent <= quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = quadruple::power_of_2(exponent);
                auto multiplied = first_power * quadruple{2};
                auto next_power = quadruple::power_of_2(exponent + 1);
                REQUIRE(multiplied == next_power);
            }
        }
        SECTION("division") {
            for (int32_t exponent = quadruple_min_representable_pow2 - 1; exponent < quadruple_max_representable_pow2;
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
            for (int32_t exponent = quadruple_min_representable_pow2; exponent <= quadruple_max_representable_pow2;
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
            for (int32_t exponent = quadruple_min_representable_pow2; exponent <= quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = -quadruple::power_of_2(exponent);
                auto multiplied = first_power * quadruple{2};
                auto next_power = -quadruple::power_of_2(exponent + 1);
                REQUIRE(multiplied == next_power);
            }
        }
        SECTION("division") {
            for (int32_t exponent = quadruple_min_representable_pow2 - 1; exponent < quadruple_max_representable_pow2;
                 exponent++) {
                auto first_power = -quadruple::power_of_2(exponent);
                auto next_power = -quadruple::power_of_2(exponent + 1);
                auto divided = next_power / quadruple{2};
                REQUIRE(divided == first_power);
            }
        }
    }
}
