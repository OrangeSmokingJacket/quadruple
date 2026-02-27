#pragma once
#include <cstdint>
#include <bitset>
#include <cmath>
#include <cstring>
#include <type_traits>

constexpr size_t float_exponent_size = 8;
constexpr size_t double_exponent_size = 11;
constexpr size_t quadruple_exponent_size = 15;
constexpr size_t float_mantissa_size = 23;
constexpr size_t double_mantissa_size = 52;
constexpr size_t quadruple_mantissa_size = 112;

constexpr uint16_t float_exponent_filler = 0b0011111110000000;
constexpr uint16_t double_exponent_filler = 0b0011110000000000;

constexpr uint32_t float_exponent_max_mask = 0b01111111100000000000000000000000;
constexpr uint32_t float_exponent_min_mask = ~float_exponent_max_mask;
constexpr uint64_t double_exponent_max_mask = 0b0111111111110000000000000000000000000000000000000000000000000000;
constexpr uint64_t double_exponent_min_mask = ~double_exponent_max_mask;
constexpr uint16_t quadruple_exponent_min = 0b0000000000000000;
constexpr uint16_t quadruple_exponent_max = 0b0111111111111111;

constexpr uint64_t float_mantissa_mask = (uint64_t{1} << float_mantissa_size) - 1;
constexpr uint64_t double_mantissa_mask = (uint64_t{1} << double_mantissa_size) - 1;

// mantissa_calc
constexpr size_t upper_bit_size = quadruple_mantissa_size - sizeof(uint64_t) * 8 + 1;

template<class T>
concept Unsigned = requires {
    std::is_unsigned_v<T>;
};

template<class T>
concept FloatingPoint = requires {
    std::is_floating_point_v<T>;
};

template<class T, size_t N>
concept ValidBitIndex = sizeof(T) * 8 > N;

template <typename T>
constexpr size_t bit_size_of() noexcept {
    return sizeof(T) * 8;
}

template <typename T>
constexpr size_t bit_size_of(T&& value) noexcept {
    return sizeof(value) * 8;
}

template <FloatingPoint T>
constexpr bool is_sNaN(T value) noexcept {
    constexpr T sNaN = std::numeric_limits<T>::signaling_NaN();
    constexpr T minus_sNaN = -sNaN;
    return std::memcmp(&value, &sNaN, sizeof(T)) == 0 ||
           std::memcmp(&value, &minus_sNaN, sizeof(T)) == 0;
}

template <FloatingPoint T>
constexpr bool is_qNaN(T value) noexcept {
    constexpr T qNaN = std::numeric_limits<T>::quiet_NaN();
    constexpr T minus_qNaN = -qNaN;
    return std::memcmp(&value, &qNaN, sizeof(T)) == 0 ||
           std::memcmp(&value, &minus_qNaN, sizeof(T)) == 0;
}

template <Unsigned T, size_t N> requires ValidBitIndex<T, N>
constexpr T single_bit_mask() noexcept{
    return T{1} << (sizeof(T) * 8 - N - 1);
}

template <Unsigned T>
constexpr T value_sing_mask() noexcept {
    return single_bit_mask<T, 0>();
}

template <Unsigned T>
constexpr T exponent_sing_mask() noexcept {
    return single_bit_mask<T, 1>();
}

template <Unsigned T>
constexpr T sign_bits_mask() noexcept {
    return value_sing_mask<T>() | exponent_sing_mask<T>();
}

template <Unsigned T>
constexpr T invert_bit_mask(T mask) noexcept {
    return static_cast<T>(~mask);
}

template <Unsigned T>
constexpr void copy_sign_bits(T& dest, T source) noexcept {
    dest |= static_cast<T>(source & sign_bits_mask<T>());
}

template <Unsigned T, size_t N> requires ValidBitIndex<T, N>
constexpr bool is_bit_set(T value) noexcept {
    return (value & single_bit_mask<T, N>()) != 0;
}

template <Unsigned T>
constexpr bool is_exponent_sing_bit_set(T value) noexcept {
    return (value & exponent_sing_mask<T>()) != 0;
}

template <Unsigned T, Unsigned U>
constexpr void copy_sign_bits(T& dest, U source) noexcept {
    if constexpr (sizeof(T) == sizeof(U)) {
        dest |= source & sign_bits_mask<U>();
    } else if constexpr (sizeof(T) > sizeof(U)) {
        dest |= static_cast<T>(source & sign_bits_mask<U>()) << (sizeof(T) - sizeof(U)) * 8;
    } else {
        dest |= static_cast<T>((source & sign_bits_mask<U>()) >> (sizeof(U) - sizeof(T)) * 8);
    }
}

constexpr uint16_t exponent_to_uint16(uint16_t exponent_value) noexcept {
    return exponent_value & invert_bit_mask(single_bit_mask<uint16_t, 0>());
}

constexpr int exponent_difference(uint16_t lhs, uint16_t rhs) noexcept {
    return exponent_to_uint16(lhs) - exponent_to_uint16(rhs);
}