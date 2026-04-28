#pragma once
#include <bit>
#include <bitset>
#include <cstdint>
#include <type_traits>

#if defined(IMPLICIT_CASTS)
    #define OPTIONAL_EXPLICIT()
#else
    #define OPTIONAL_EXPLICIT() explicit
#endif

constexpr size_t float_exponent_size = 8;
constexpr size_t double_exponent_size = 11;
constexpr size_t quadruple_exponent_size = 15;
constexpr size_t float_mantissa_size = 23;
constexpr size_t double_mantissa_size = 52;
constexpr size_t quadruple_mantissa_size = 112;

constexpr uint32_t float_exponent_max_mask = 0x7F800000;
constexpr uint32_t float_exponent_min_mask = ~float_exponent_max_mask;
constexpr uint64_t double_exponent_max_mask = 0x7FF0000000000000;
constexpr uint64_t double_exponent_min_mask = ~double_exponent_max_mask;

constexpr uint64_t float_mantissa_mask = (uint64_t{1} << float_mantissa_size) - 1;
constexpr uint64_t double_mantissa_mask = (uint64_t{1} << double_mantissa_size) - 1;

namespace exponent_values {
    constexpr uint16_t quadruple_exponent_max = 0x7FFF;
    constexpr uint16_t quadruple_exponent_min = 0;
    constexpr uint16_t quadruple_exponent_bias = 0x3FFF;
    constexpr uint16_t max_float_exponent = quadruple_exponent_max / 2 + (uint16_t{1} << (float_exponent_size - 1));
    constexpr uint16_t max_double_exponent = quadruple_exponent_max / 2 + (uint16_t{1} << (double_exponent_size - 1));
    constexpr uint16_t min_float_exponent = quadruple_exponent_max / 2 - (uint16_t{1} << (float_exponent_size - 1)) + 1;
    constexpr uint16_t min_double_exponent =
        quadruple_exponent_max / 2 - (uint16_t{1} << (double_exponent_size - 1)) + 1;
} // namespace exponent_values

constexpr uint64_t float_exponent_filler = 0x3F80000000000000;
constexpr uint64_t double_exponent_filler = 0x3C00000000000000;
constexpr uint64_t float_subnormal_exponent_filler = 0x3F81000000000000;
constexpr uint64_t double_subnormal_exponent_filler = 0x3C01000000000000;

constexpr uint64_t upper_mantissa_mask = 0x0000FFFFFFFFFFFF;
constexpr uint64_t implied_bit_mask = 0x0001000000000000;
constexpr uint64_t sign_bit_mask = 0x8000000000000000;
constexpr uint64_t sign_bits_mask = 0xC000000000000000;
constexpr uint64_t zero_mask = sign_bit_mask;
constexpr uint64_t subnormal_mask = 0x7FFF000000000000;

constexpr uint64_t quadruple_exponent_max = 0x7FFF000000000000;
constexpr uint64_t quadruple_exponent_min = 0;
constexpr uint64_t quadruple_exponent_bias = 0x3FFF000000000000;

constexpr int32_t quadruple_min_normal_representable_pow2 =
    1 - static_cast<int32_t>(exponent_values::quadruple_exponent_bias);
constexpr int32_t quadruple_min_representable_pow2 =
    quadruple_min_normal_representable_pow2 - static_cast<int32_t>(quadruple_mantissa_size);
constexpr int32_t quadruple_max_representable_pow2 = static_cast<int32_t>(exponent_values::quadruple_exponent_bias);

#if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)

constexpr unsigned __int128 max_representable_uint128 = (unsigned __int128){1} << quadruple_mantissa_size;
constexpr unsigned __int128 min_representable_uint128 = (unsigned __int128){0};
constexpr __int128 max_representable_int128 = static_cast<__int128>(max_representable_uint128);
constexpr __int128 min_representable_int128 = -max_representable_int128;

#endif

// mantissa_calc
constexpr size_t upper_bit_size = quadruple_mantissa_size - sizeof(uint64_t) * 8 + 1;

template <class T>
concept Unsigned = requires { std::is_unsigned_v<T>; };

template <class T>
concept FloatingPoint = requires {
    std::is_floating_point_v<T>;
    sizeof(T) == sizeof(uint32_t) || sizeof(T) == sizeof(uint64_t);
};

template <class T, size_t N>
concept ValidBitIndex = sizeof(T) * 8 > N;

template <typename T>
constexpr size_t bit_size_of() noexcept {
    return sizeof(T) * 8;
}

template <typename T>
constexpr size_t bit_size_of(T&& value) noexcept {
    return sizeof(value) * 8;
}

template <Unsigned T, size_t N>
requires ValidBitIndex<T, N>
constexpr T single_bit_mask() noexcept {
    return T{1} << (sizeof(T) * 8 - N - 1);
}

template <Unsigned T>
constexpr T single_bit_mask(size_t N) noexcept {
    return T{1} << (sizeof(T) * 8 - N - 1);
}

template <Unsigned T>
constexpr T value_sign_mask() noexcept {
    return single_bit_mask<T, 0>();
}

template <Unsigned T, size_t N>
requires ValidBitIndex<T, N>
constexpr bool is_bit_set(T value) noexcept {
    return (value & single_bit_mask<T, N>()) != 0;
}

template <Unsigned T>
constexpr bool is_exponent_sign_bit_set(T value) noexcept {
    return (value & single_bit_mask<T, 1>()) != 0;
}

template <Unsigned T>
constexpr int most_significant_bit_position(T value) noexcept {
    T bit_mask = single_bit_mask<T, 0>();
    for (int i = 0; i < static_cast<int>(sizeof(value) * 8); i++) {
        if ((value & bit_mask) != 0) {
            return i;
        }
        bit_mask >>= 1;
    }
    return sizeof(value) * 8;
}

template <Unsigned T>
constexpr int least_significant_bit_position(T value) noexcept {
    T bit_mask = single_bit_mask<T, 63>();
    for (int i = static_cast<int>(sizeof(value) * 8) - 1; i >= 0; i--) {
        if ((value & bit_mask) != 0) {
            return i;
        }
        bit_mask <<= 1;
    }
    return -1;
}

constexpr uint16_t exponent_to_uint16(uint64_t exponent_value) noexcept {
    return static_cast<uint16_t>((exponent_value & ~single_bit_mask<uint64_t, 0>()) >>
                                 ((sizeof(uint64_t) - sizeof(uint16_t)) * 8));
}

template <FloatingPoint T>
constexpr bool is_sNaN(T value) noexcept {
    constexpr T sNaN = std::numeric_limits<T>::signaling_NaN();
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return (std::bit_cast<uint32_t>(sNaN) & std::bit_cast<uint32_t>(value)) == std::bit_cast<uint32_t>(sNaN);
    } else {
        return (std::bit_cast<uint64_t>(sNaN) & std::bit_cast<uint64_t>(value)) == std::bit_cast<uint64_t>(sNaN);
    }
}

template <FloatingPoint T>
constexpr bool is_qNaN(T value) noexcept {
    constexpr T qNaN = std::numeric_limits<T>::quiet_NaN();
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return (std::bit_cast<uint32_t>(qNaN) & std::bit_cast<uint32_t>(value)) == std::bit_cast<uint32_t>(qNaN);
    } else {
        return (std::bit_cast<uint64_t>(qNaN) & std::bit_cast<uint64_t>(value)) == std::bit_cast<uint64_t>(qNaN);
    }
}

// avoids triggering FE_INVALID
template <FloatingPoint T>
constexpr T negative_sNaN() noexcept {
    constexpr T sNaN = std::numeric_limits<T>::signaling_NaN();
    if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return std::bit_cast<T>(std::bit_cast<uint32_t>(sNaN) ^ single_bit_mask<uint32_t, 0>());
    } else {
        return std::bit_cast<T>(std::bit_cast<uint64_t>(sNaN) ^ single_bit_mask<uint64_t, 0>());
    }
}
