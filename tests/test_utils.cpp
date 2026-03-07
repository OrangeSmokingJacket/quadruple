#include <catch2/catch_all.hpp>
#include "test_utils.hpp"

namespace {
    template <typename T, typename U>
    concept FloatUintPair = requires
    {
        std::is_floating_point_v<T>;
        std::is_unsigned_v<U>;
        sizeof(T) == sizeof(U);
    };

    template <typename T, typename U, bool allow_subnormals, typename Generator> requires FloatUintPair<T, U>
    std::vector<T> generate_sequence(Generator& generator, U mask, size_t count) {
        std::vector<T> result;
        result.reserve(count);
        for (size_t i = 0; i < count; i++) {
            if constexpr (!allow_subnormals) {
                T val;
                do {
                    val = std::bit_cast<T>(static_cast<U>(generator()) & mask);
                } while (std::fpclassify(val) == FP_SUBNORMAL);
                result.emplace_back(val);
            } else {
                result.emplace_back(std::bit_cast<T>(static_cast<U>(generator()) & mask));
            }
        }
        return result;
    }
}

template <>
inline std::vector<float> generate_normal_numbers(size_t count) {
    static constexpr uint32_t float_mantissa_mask = 0xFFFFFFFF;
    std::mt19937 generator{};
    return generate_sequence<float, uint32_t, false>(generator, float_mantissa_mask, count);
}

template <>
inline std::vector<double> generate_normal_numbers(size_t count) {
    static constexpr uint64_t double_mantissa_mask = 0xFFFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_sequence<double, uint64_t, false>(generator, double_mantissa_mask, count);
}

template <>
inline std::vector<float> generate_subnormal_numbers(size_t count) {
    static constexpr uint32_t float_mantissa_mask = 0x007FFFFF;
    std::mt19937 generator{};
    return generate_sequence<float, uint32_t, true>(generator, float_mantissa_mask, count);
}

template <>
inline std::vector<double> generate_subnormal_numbers(size_t count) {
    static constexpr uint64_t double_mantissa_mask = 0x000FFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_sequence<double, uint64_t, true>(generator, double_mantissa_mask, count);
}

TEMPLATE_TEST_CASE("test utils", "[utils]", float, double) {

    SECTION("test repeatability") {
        SECTION("normal") {
            auto first_gen = generate_normal_numbers<TestType>(test_size);
            auto second_gen = generate_normal_numbers<TestType>(test_size);

            // check that bits are the same
            REQUIRE(std::memcmp(first_gen.data(), second_gen.data(), sizeof(TestType) * test_size) == 0);
        }
        SECTION("subnormal") {
            auto first_gen = generate_subnormal_numbers<TestType>(test_size);
            auto second_gen = generate_subnormal_numbers<TestType>(test_size);

            // check that bits are the same
            REQUIRE(std::memcmp(first_gen.data(), second_gen.data(), sizeof(TestType) * test_size) == 0);
        }
    }
}