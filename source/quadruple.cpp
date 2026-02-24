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
constexpr size_t quadruple_exponent_size = 15;
constexpr size_t float_mantissa_size = 23;
constexpr size_t double_mantissa_size = 52;
constexpr size_t quadruple_mantissa_size = 112;

constexpr uint16_t float_exponent_filler = 0b0011111110000000;
constexpr uint16_t double_exponent_filler = 0b0011110000000000;

constexpr uint64_t float_mantissa_mask = (uint64_t{1} << float_mantissa_size) - 1;
constexpr uint64_t double_mantissa_mask = (uint64_t{1} << double_mantissa_size) - 1;

// mantissa_calc
constexpr size_t upper_bit_size = quadruple_mantissa_size - sizeof(uint64_t) * 8 + 1;

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

quadruple::quadruple(float value) noexcept {
    auto flat_value = std::bit_cast<uint32_t>(value);
    // copy mantissa
    uint64_t mantissa_val = static_cast<uint64_t>(flat_value) & float_mantissa_mask;
    mantissa_val <<= sizeof(mantissa_val) * 8 - float_mantissa_size;
    std::memcpy(&mantissa1_,
        reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        sizeof(mantissa1_) + sizeof(mantissa2_));
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
        exponent_ |= float_exponent_filler;
    }
}

quadruple::quadruple(double value) noexcept {
    auto flat_value = std::bit_cast<uint64_t>(value);
    // copy mantissa
    uint64_t mantissa_val = flat_value & double_mantissa_mask;
    mantissa_val <<= sizeof(mantissa_val) * 8 - double_mantissa_size;
    std::memcpy(&mantissa1_,
        reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        sizeof(mantissa1_) + sizeof(mantissa2_));
    mantissa_val <<= (sizeof(mantissa1_) + sizeof(mantissa2_)) * 8;
    mantissa3_ = mantissa_val;
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
        exponent_ |= double_exponent_filler;
    }
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
    uint64_t mantissa_val{0};
    std::memcpy(reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        &mantissa1_,
        sizeof(mantissa1_) + sizeof(mantissa2_));
    mantissa_val >>= sizeof(mantissa_val) * 8 - float_mantissa_size;

    // combine
    std::memcpy(reinterpret_cast<char*>(&float_bits) + (sizeof(float_bits) - sizeof(float_exp)),
        &float_exp,
        sizeof(float_exp));
    float_bits |= static_cast<uint32_t>(mantissa_val);

    auto result = std::bit_cast<float>(float_bits);
    if (is_bit_set<decltype(mantissa2_), float_mantissa_size>(mantissa2_)) {
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
    uint64_t mantissa_val = mantissa3_;
    mantissa_val >>= (sizeof(mantissa1_) + sizeof(mantissa2_)) * 8;
    std::memcpy(reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        &mantissa1_,
        sizeof(mantissa1_) + sizeof(mantissa2_));
    mantissa_val >>= sizeof(mantissa_val) * 8 - double_mantissa_size;

    // combine
    std::memcpy(reinterpret_cast<char*>(&double_bits) + (sizeof(double_bits) - sizeof(double_exp)),
        &double_exp,
        sizeof(double_exp));
    double_bits |= mantissa_val;

    auto result = std::bit_cast<double>(double_bits);
    if (is_bit_set<decltype(mantissa3_), sizeof(mantissa3_) * 8 - double_mantissa_size>(mantissa3_)) {
        if (is_bit_set<decltype(double_bits), 0>(double_bits)) {
            return std::nextafter(result, -std::numeric_limits<double>::infinity());
        } else {
            return std::nextafter(result, std::numeric_limits<double>::infinity());
        }
    } else {
        return result;
    }
}

bool quadruple::is_nan() const noexcept {
    static constexpr uint16_t nan_mask = invert_bit_mask(single_bit_mask<uint16_t, 0>());
    return static_cast<uint16_t>(exponent_ & nan_mask) == nan_mask &&
        (mantissa1_ != 0 || mantissa2_ != 0 || mantissa3_ != 0);
}

bool quadruple::is_subnormal() const noexcept {
    static constexpr uint16_t subnormal_mask = single_bit_mask<uint16_t, 0>();
    return static_cast<uint16_t>(exponent_ | subnormal_mask) == subnormal_mask;
}

bool quadruple::signbit() const noexcept {
    return (exponent_ & single_bit_mask<decltype(exponent_), 0>()) != 0;
}

quadruple quadruple::nan() noexcept {
    return {0x7FFF,
            std::numeric_limits<uint16_t>::max(),
            std::numeric_limits<uint32_t>::max(),
            std::numeric_limits<uint64_t>::max()};
}

quadruple quadruple::infinity() noexcept {
    return {0x7FFF, 0, 0, 0};
}

quadruple quadruple::operator+() const noexcept {
    return *this;
}

quadruple quadruple::operator-() const noexcept {
    return {static_cast<uint16_t>(exponent_ ^ single_bit_mask<uint16_t, 0>()),
        mantissa1_,
        mantissa2_,
        mantissa3_};
}

quadruple quadruple::operator+(const quadruple& rhs) const noexcept {
    if (is_nan() || rhs.is_nan()) {
        return nan();
    }

    auto this_sign = signbit();
    if (this_sign != rhs.signbit()) {
        if (this_sign) {
            return rhs - -(*this);
        } else {
            return operator-(-rhs);
        }
    }
    // TODO: this will fail with subnormal numbers
    auto lhs_mantissa = convert_mantissa();
    auto rhs_mantissa = rhs.convert_mantissa();
    auto exp_diff = exponent_difference(exponent_, rhs.exponent_);

    if (exp_diff < 0) {
        lhs_mantissa.shift_right(static_cast<uint32_t>(-exp_diff));
    } else if (exp_diff > 0) {
        rhs_mantissa.shift_right(static_cast<uint32_t>(exp_diff));
    }

    auto res_mantissa = lhs_mantissa + rhs_mantissa;
    if (res_mantissa.is_zero()) {
        return quadruple{};
    }
    auto msb = res_mantissa.most_significant_bit_position();
    // 0 adjustment if msb is 64 - upper_bit_size
    int exponent_adj = static_cast<int>(sizeof(mantissa_calc::lower) * 8 - upper_bit_size) - msb;
    res_mantissa.normalize(exponent_adj);
    auto adjusted_exponent = static_cast<uint16_t>(exponent_to_uint16(exponent_) + static_cast<int16_t>(exponent_adj));
    if ((adjusted_exponent & single_bit_mask<uint16_t, 0>()) != 0) {
        // exponent overflow
        if (this_sign) {
            return -infinity();
        } else {
            return infinity();
        }
    }

    auto res = quadruple{adjusted_exponent, 0, 0, res_mantissa.lower};
    std::memcpy(&res.mantissa1_, &res_mantissa.upper, sizeof(res.mantissa1_) + sizeof(res.mantissa2_));
    if (this_sign) {
        res.exponent_ |= single_bit_mask<decltype(res.exponent_), 0>();
    }
    return res;
}

quadruple quadruple::operator-(const quadruple& rhs) const noexcept {
    if (is_nan() || rhs.is_nan()) {
        return nan();
    }

    auto this_sign = signbit();
    if (this_sign != rhs.signbit()) {
        return operator+(-rhs);
    }
    // TODO: this will fail with subnormal numbers
    auto lhs_mantissa = convert_mantissa();
    auto rhs_mantissa = rhs.convert_mantissa();
    auto exp_diff = exponent_difference(exponent_, rhs.exponent_);

    if (exp_diff < 0) {
        lhs_mantissa.shift_right(static_cast<uint32_t>(-exp_diff));
    } else if (exp_diff > 0) {
        rhs_mantissa.shift_right(static_cast<uint32_t>(exp_diff));
    }

    mantissa_calc res_mantissa;
    bool flipped_sign = false;
    if (lhs_mantissa < rhs_mantissa) {
        res_mantissa = rhs_mantissa - lhs_mantissa;
        this_sign = !this_sign;
        flipped_sign = true;
    } else {
        res_mantissa = lhs_mantissa - rhs_mantissa;
    }
    if (res_mantissa.is_zero()) {
        return quadruple{};
    }
    auto msb = res_mantissa.most_significant_bit_position();
    // 0 adjustment if msb is 64 - upper_bit_size
    int exponent_adj = static_cast<int>(sizeof(mantissa_calc::lower) * 8 - upper_bit_size) - msb;
    res_mantissa.normalize(exponent_adj);
    uint16_t adjusted_exponent;
    if (flipped_sign) {
        adjusted_exponent = static_cast<uint16_t>(exponent_to_uint16(rhs.exponent_) + static_cast<int16_t>(exponent_adj));
    } else {
        adjusted_exponent = static_cast<uint16_t>(exponent_to_uint16(exponent_) + static_cast<int16_t>(exponent_adj));
    }
    if ((adjusted_exponent & single_bit_mask<uint16_t, 0>()) != 0) {
        // exponent overflow
        if (this_sign) {
            return -infinity();
        } else {
            return infinity();
        }
    }

    auto res = quadruple{adjusted_exponent, 0, 0, res_mantissa.lower};
    std::memcpy(&res.mantissa1_, &res_mantissa.upper, sizeof(res.mantissa1_) + sizeof(res.mantissa2_));
    if (this_sign) {
        res.exponent_ |= single_bit_mask<decltype(res.exponent_), 0>();
    }
    return res;
}

quadruple::quadruple(uint16_t exponent, uint16_t mantissa1, uint32_t mantissa2, uint64_t mantissa3) noexcept
    : exponent_(exponent)
    , mantissa1_(mantissa1)
    , mantissa2_(mantissa2)
    , mantissa3_(mantissa3) {}

bool quadruple::mantissa_calc::is_zero() const noexcept {
    return upper == 0 && lower == 0;
}

// returns 128, if there is no bits set
int quadruple::mantissa_calc::most_significant_bit_position() const noexcept {
    uint64_t bit_mask = single_bit_mask<uint64_t, 0>();
    for (int i = 0; i < static_cast<int>(sizeof(upper) * 8); i++) {
        if ((upper & bit_mask) != 0) {
            return i;
        }
        bit_mask >>= 1;
    }
    bit_mask = single_bit_mask<uint64_t, 0>();
    for (int i = 0; i < static_cast<int>(sizeof(lower) * 8); i++) {
        if ((lower & bit_mask) != 0) {
            return i + static_cast<int>(sizeof(upper) * 8);
        }
        bit_mask >>= 1;
    }
    return (sizeof(upper) + sizeof(lower)) * 8;
}

void quadruple::mantissa_calc::normalize(int adjustment) noexcept {
    if (adjustment == 0) {
        return;
    }
    if (adjustment > 0) {
        shift_right(static_cast<uint32_t>(adjustment));
    } else {
        shift_left(static_cast<uint32_t>(-adjustment));
    }
}

void quadruple::mantissa_calc::shift_left(uint32_t amount) noexcept {
    assert(amount < sizeof(upper) * 8 && "shift left is not supposed to get a this big of a shift");
    uint64_t lower_store = lower;
    lower_store >>= sizeof(lower_store) * 8 - amount;
    lower <<= amount;
    upper <<= amount;
    upper |= lower_store;
}

void quadruple::mantissa_calc::shift_right(uint32_t amount) noexcept {
    if (amount > quadruple_mantissa_size) {
        upper = 0;
        lower = 0;
        return;
    }
    if (amount > upper_bit_size) {
        amount -= static_cast<uint32_t>(upper_bit_size);
        lower = upper << (sizeof(upper) * 8 - upper_bit_size);
        lower >>= amount;
        upper = 0;
    } else {
        uint64_t upper_store = upper;
        upper_store <<= sizeof(upper_store) * 8 - amount;
        upper >>= amount;
        lower >>= amount;
        lower |= upper_store;
    }
}

quadruple::mantissa_calc quadruple::mantissa_calc::operator+(const mantissa_calc& rhs) const noexcept {
    mantissa_calc result;
    result.upper = upper + rhs.upper;
    result.lower = lower + rhs.lower;
    if (result.lower < lower) {
        // there was overflow
        ++result.upper;
    }
    return result;
}

quadruple::mantissa_calc quadruple::mantissa_calc::operator-(const mantissa_calc& rhs) const noexcept {
    mantissa_calc result;
    result.upper = upper - rhs.upper;
    result.lower = lower - rhs.lower;
    if (result.lower > lower) {
        // there was overflow
        --result.upper;
    }
    return result;
}

bool quadruple::mantissa_calc::operator<(const mantissa_calc& rhs) const noexcept {
    return upper < rhs.upper || (upper == rhs.upper && lower < rhs.lower);
}

quadruple::mantissa_calc quadruple::convert_mantissa() const {
    static constexpr uint64_t implied_mask = single_bit_mask<uint64_t, sizeof(exponent_) * 8 - 1>();
    mantissa_calc result;
    std::memcpy(&result.upper, &mantissa1_, sizeof(mantissa1_) + sizeof(mantissa2_));
    if (!is_subnormal()) {
        result.upper |= implied_mask;
    }
    result.lower = mantissa3_;
    return result;
}
