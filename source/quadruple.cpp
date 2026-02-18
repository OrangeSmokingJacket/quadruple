#include "quadruple.hpp"

#include <bit>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstring>
#include <type_traits>

static_assert(sizeof(quadruple) == 16);
static_assert(alignof(quadruple) == 8);
static_assert(std::is_default_constructible_v<quadruple>);
static_assert(std::is_trivially_destructible_v<quadruple>);
static_assert(std::is_trivially_copyable_v<quadruple>);
static_assert(std::is_trivially_copy_assignable_v<quadruple>);
static_assert(std::is_trivially_move_constructible_v<quadruple>);
static_assert(std::is_trivially_move_assignable_v<quadruple>);

constexpr size_t float_exponent_size = 8;
constexpr size_t double_exponent_size = 11;

constexpr uint16_t float_1_filler = 0b0011111110000000;
constexpr uint16_t double_1_filler = 0b0011110000000000;

template<class T>
concept Unsigned = requires {
    std::is_unsigned_v<T>;
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

quadruple::quadruple(float value) noexcept {
    auto flat_value = std::bit_cast<uint32_t>(value);
    // remove sing bits
    flat_value <<= 2;
    std::memcpy(&exponent_,
        reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(exponent_)),
        sizeof(exponent_));
    // align exponent bits
    exponent_ >>= bit_size_of(exponent_) - float_exponent_size + 1;
    // place sing bits back
    copy_sign_bits(exponent_, std::bit_cast<uint32_t>(value));
    if (!is_exponent_sing_bit_set(std::bit_cast<uint32_t>(value))) {
        exponent_ |= float_1_filler;
    }

    // remove exponent
    flat_value <<= float_exponent_size - 1;
    std::memcpy(&mantissa1_,
        reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(mantissa1_)),
        sizeof(mantissa1_));
    // remove mantissa1_ part
    flat_value <<= bit_size_of(mantissa1_);
    std::memcpy(&mantissa2_, &flat_value, sizeof(uint32_t));
}

quadruple::quadruple(double value) noexcept {
    auto flat_value = std::bit_cast<uint64_t>(value);
    // remove sing bits
    flat_value <<= 2;
    std::memcpy(&exponent_,
        reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(exponent_)),
        sizeof(exponent_));
    // align exponent bits
    exponent_ >>= bit_size_of(exponent_) - double_exponent_size + 1;
    // place sing bits back
    copy_sign_bits(exponent_, std::bit_cast<uint64_t>(value));
    if (!is_exponent_sing_bit_set(std::bit_cast<uint64_t>(value))) {
        exponent_ |= double_1_filler;
    }

    // remove exponent
    flat_value <<= double_exponent_size - 1;
    std::memcpy(&mantissa1_,
        reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(mantissa1_)),
        sizeof(mantissa1_));
    // remove mantissa1_ part
    flat_value <<= bit_size_of(mantissa1_);
    std::memcpy(&mantissa2_,
        reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(mantissa2_)),
        sizeof(uint32_t));
    // remove mantissa2_ part
    flat_value <<= bit_size_of(mantissa2_);
    std::memcpy(&mantissa3_, &flat_value, sizeof(uint64_t));
}

quadruple::operator float() const noexcept {
    uint32_t float_bits{0};

    // create exponent
    uint16_t float_exp = exponent_;
    // shift left by extra 2 bits to remove signs
    float_exp <<= bit_size_of(exponent_) - float_exponent_size + 1;
    float_exp >>= 2;
    copy_sign_bits(float_exp, exponent_);

    // create mantissa
    uint32_t float_mantissa{0};
    std::memcpy(reinterpret_cast<char*>(&float_mantissa) + sizeof(mantissa1_),
        &mantissa1_,
        sizeof(mantissa1_));
    std::memcpy(&float_mantissa,
        reinterpret_cast<const char*>(&mantissa2_) + sizeof(mantissa1_),
        sizeof(float_mantissa) - sizeof(mantissa1_));
    float_mantissa >>= float_exponent_size + 1;

    // combine
    std::memcpy(reinterpret_cast<char*>(&float_bits) + (sizeof(float_bits) - sizeof(float_exp)),
        &float_exp,
        sizeof(float_exp));
    float_bits |= float_mantissa;

    auto result = std::bit_cast<float>(float_bits);
    if (is_bit_set<decltype(mantissa2_), 7>(mantissa2_)) {
        if (is_bit_set<decltype(float_bits), 0>(float_bits)) {
            return std::nextafter(result, -std::numeric_limits<float>::infinity());
        } else {
            return std::nextafter(result, std::numeric_limits<float>::infinity());
        }
    } else {
        return result;
    }
}

quadruple::operator double() const noexcept {
    uint64_t double_bits{0};

    // create exponent
    uint16_t double_exp = exponent_;
    double_exp <<= bit_size_of(exponent_) - double_exponent_size + 1;
    double_exp >>= 2;
    copy_sign_bits(double_exp, exponent_);

    // create mantissa
    uint64_t double_mantissa{0};
    std::memcpy(reinterpret_cast<char*>(&double_mantissa) + (sizeof(double_mantissa) - sizeof(mantissa1_)),
        &mantissa1_,
        sizeof(mantissa1_));
    std::memcpy(reinterpret_cast<char*>(&double_mantissa) + (sizeof(double_mantissa) - sizeof(mantissa1_) - sizeof(mantissa2_)),
        &mantissa2_,
        sizeof(mantissa2_));
    std::memcpy(&double_mantissa,
        reinterpret_cast<const char*>(&mantissa3_) + sizeof(mantissa1_) + sizeof(mantissa2_),
        sizeof(double_mantissa) - sizeof(mantissa1_) - sizeof(mantissa2_));
    double_mantissa >>= double_exponent_size + 1;

    // combine
    std::memcpy(reinterpret_cast<char*>(&double_bits) + (sizeof(double_bits) - sizeof(double_exp)),
        &double_exp,
        sizeof(double_exp));
    double_bits |= double_mantissa;

    auto result = std::bit_cast<double>(double_bits);
    if (is_bit_set<decltype(mantissa3_), 4>(mantissa3_)) {
        if (is_bit_set<decltype(double_bits), 0>(double_bits)) {
            return std::nextafter(result, -std::numeric_limits<double>::infinity());
        } else {
            return std::nextafter(result, std::numeric_limits<double>::infinity());
        }
    } else {
        return result;
    }
}
