#include "test_utils.hpp"
#include "quadruple.hpp"
#include <array>
#include <random>

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

    template <bool allow_subnormals, typename Generator>
    std::vector<quadruple> generate_sequence(Generator& generator, uint64_t mask, size_t count) {
        std::array<std::byte, sizeof(quadruple)> bits;

        std::vector<quadruple> result;
        result.reserve(count);
        for (size_t i = 0; i < count; i++) {
            if constexpr (!allow_subnormals) {
                quadruple val;
                do {
                    *reinterpret_cast<uint64_t*>(&bits[0]) = static_cast<uint64_t>(generator());
                    *reinterpret_cast<uint64_t*>(&bits[8]) = static_cast<uint64_t>(generator()) & mask;
                    val = std::bit_cast<quadruple>(bits);
                } while (val.is_subnormal());
                result.emplace_back(val);
            } else {
                *reinterpret_cast<uint64_t*>(&bits[0]) = static_cast<uint64_t>(generator());
                *reinterpret_cast<uint64_t*>(&bits[8]) = static_cast<uint64_t>(generator()) & mask;
                result.emplace_back(std::bit_cast<quadruple>(bits));
            }
        }
        return result;
    }

    template<typename T, typename Generator> requires (sizeof(T) <= sizeof(uint64_t))
    std::vector<T> generate_integer_sequence(Generator& generator, uint64_t mask, size_t count) {
        std::vector<T> result;
        result.reserve(count);
        for (size_t i = 0; i < count; i++) {
            result.emplace_back(static_cast<T>(static_cast<uint64_t>(generator()) & mask));
        }
        return result;
    }

#if defined(__SIZEOF_INT128__)

    template<typename T, typename Generator>  requires (sizeof(T) == __SIZEOF_INT128__)
    std::vector<T> generate_integer_sequence(Generator& generator, uint64_t mask, size_t count) {
        struct low_high {
            uint64_t low;
            uint64_t high;
        };
        std::vector<T> result;
        result.reserve(count);
        for (size_t i = 0; i < count; i++) {
            low_high data;
            data.low = static_cast<uint64_t>(generator());
            data.high = static_cast<uint64_t>(generator()) & mask;
            result.emplace_back(std::bit_cast<T>(data));
        }
        return result;
    }

#endif

}

template <>
std::vector<int8_t> generate_normal_numbers(size_t count) {
    static constexpr uint8_t int8_mask = 0x7F;
    std::mt19937 generator{};
    return generate_integer_sequence<int8_t>(generator, int8_mask, count);
}

template <>
std::vector<int16_t> generate_normal_numbers(size_t count) {
    static constexpr uint16_t int16_mask = 0x7FFF;
    std::mt19937 generator{};
    return generate_integer_sequence<int16_t>(generator, int16_mask, count);
}

template <>
std::vector<int32_t> generate_normal_numbers(size_t count) {
    static constexpr uint32_t int32_mask = 0x7FFFFFFF;
    std::mt19937 generator{};
    return generate_integer_sequence<int32_t>(generator, int32_mask, count);
}

template <>
std::vector<int64_t> generate_normal_numbers(size_t count) {
    static constexpr uint64_t int64_mask = 0x7FFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_integer_sequence<int64_t>(generator, int64_mask, count);
}

template <>
std::vector<uint8_t> generate_normal_numbers(size_t count) {
    static constexpr uint8_t uint8_mask = 0xFF;
    std::mt19937 generator{};
    return generate_integer_sequence<uint8_t>(generator, uint8_mask, count);
}

template <>
std::vector<uint16_t> generate_normal_numbers(size_t count) {
    static constexpr uint16_t uint16_mask = 0xFFFF;
    std::mt19937 generator{};
    return generate_integer_sequence<uint16_t>(generator, uint16_mask, count);
}

template <>
std::vector<uint32_t> generate_normal_numbers(size_t count) {
    static constexpr uint32_t uint32_mask = 0xFFFFFFFF;
    std::mt19937 generator{};
    return generate_integer_sequence<uint32_t>(generator, uint32_mask, count);
}

template <>
std::vector<uint64_t> generate_normal_numbers(size_t count) {
    static constexpr uint64_t uint64_mask = 0xFFFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_integer_sequence<uint64_t>(generator, uint64_mask, count);
}

template <>
std::vector<float> generate_normal_numbers(size_t count) {
    static constexpr uint32_t float_mantissa_mask = 0x7FFFFFFF;
    std::mt19937 generator{};
    return generate_sequence<float, uint32_t, false>(generator, float_mantissa_mask, count);
}

template <>
std::vector<double> generate_normal_numbers(size_t count) {
    static constexpr uint64_t double_mantissa_mask = 0x7FFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_sequence<double, uint64_t, false>(generator, double_mantissa_mask, count);
}

template <>
std::vector<quadruple> generate_normal_numbers(size_t count) {
    static constexpr uint64_t quadruple_upper_mask = 0x7FFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_sequence<false>(generator, quadruple_upper_mask, count);
}

template <>
std::vector<float> generate_subnormal_numbers(size_t count) {
    static constexpr uint32_t float_mantissa_mask = 0x007FFFFF;
    std::mt19937 generator{};
    return generate_sequence<float, uint32_t, true>(generator, float_mantissa_mask, count);
}

template <>
std::vector<double> generate_subnormal_numbers(size_t count) {
    static constexpr uint64_t double_mantissa_mask = 0x0000FFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_sequence<double, uint64_t, true>(generator, double_mantissa_mask, count);
}

template <>
std::vector<quadruple> generate_subnormal_numbers(size_t count) {
    static constexpr uint64_t quadruple_upper_mask = 0x0000FFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_sequence<true>(generator, quadruple_upper_mask, count);}


#if defined(__SIZEOF_INT128__)

template <>
std::vector<__int128> generate_normal_numbers(size_t count) {
    static constexpr uint64_t int128_upper_mask = 0x7FFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_integer_sequence<__int128>(generator, int128_upper_mask, count);
}

template <>
std::vector<unsigned __int128> generate_normal_numbers(size_t count) {
    static constexpr uint64_t uint128_upper_mask = 0xFFFFFFFFFFFFFFFF;
    std::mt19937_64 generator{};
    return generate_integer_sequence<unsigned __int128>(generator, uint128_upper_mask, count);
}


#endif

template <>
void remove_NaNs<quadruple>(std::vector<quadruple>& vector) {
    for (auto it = vector.begin(); it != vector.end();) {
        if (it->is_NaN()) {
            vector.erase(it);
        } else {
            ++it;
        }
    }
}