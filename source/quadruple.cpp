#include "quadruple.hpp"
#include "utils.hpp"

#include <bit>
#include <cassert>

static_assert(sizeof(quadruple) == 16);
static_assert(alignof(quadruple) == 8);
static_assert(std::is_default_constructible_v<quadruple>);
static_assert(std::is_trivially_destructible_v<quadruple>);
static_assert(std::is_trivially_copyable_v<quadruple>);
static_assert(std::is_trivially_copy_assignable_v<quadruple>);
static_assert(std::is_trivially_move_constructible_v<quadruple>);
static_assert(std::is_trivially_move_assignable_v<quadruple>);

quadruple::quadruple(float value) noexcept {
    // handle NaN
    if (is_qNaN(value)) {
        if (std::signbit(value)) {
            *this = -quiet_NaN();
        } else {
            *this = quiet_NaN();
        }
        return;
    } else if (is_sNaN(value)) {
        if (std::signbit(value)) {
            *this = -signaling_NaN();
        } else {
            *this = signaling_NaN();
        }
        return;
    }

    auto flat_value = std::bit_cast<uint32_t>(value);
    // copy mantissa
    uint64_t mantissa_val = static_cast<uint64_t>(flat_value) & float_mantissa_mask;
    mantissa_val <<= sizeof(mantissa_val) * 8 - float_mantissa_size;
    std::memcpy(&mantissa1_,
        reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        sizeof(mantissa1_) + sizeof(mantissa2_));
    // handle edge cases (max and min exponent is not a power of 2)
    if ((flat_value & float_exponent_max_mask) == float_exponent_max_mask) {
        exponent_ = quadruple_exponent_max;
        if ((flat_value & single_bit_mask<uint32_t, 0>()) == single_bit_mask<uint32_t, 0>()) {
            exponent_ |= single_bit_mask<uint16_t, 0>();
        }
    } else if ((flat_value | float_exponent_min_mask) == float_exponent_min_mask) {
        exponent_ = quadruple_exponent_min;
        if ((flat_value & single_bit_mask<uint32_t, 0>()) == single_bit_mask<uint32_t, 0>()) {
            exponent_ |= single_bit_mask<uint16_t, 0>();
        }
    } else {
        // remove sing bits
        flat_value <<= 2;
        std::memcpy(&exponent_,
            reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(exponent_)),
            sizeof(exponent_));
        // align exponent bits
        exponent_ >>= bit_size_of(exponent_) - float_exponent_size + 1;
        // place sing bit
        copy_sign_bits(exponent_, std::bit_cast<uint32_t>(value));
        if (!is_exponent_sing_bit_set(std::bit_cast<uint32_t>(value))) {
            exponent_ |= float_exponent_filler;
        }
    }
}

quadruple::quadruple(double value) noexcept {
    // handle NaN
    if (is_qNaN(value)) {
        if (std::signbit(value)) {
            *this = -quiet_NaN();
        } else {
            *this = quiet_NaN();
        }
        return;
    } else if (is_sNaN(value)) {
        if (std::signbit(value)) {
            *this = -signaling_NaN();
        } else {
            *this = signaling_NaN();
        }
        return;
    }

    auto flat_value = std::bit_cast<uint64_t>(value);
    // copy mantissa
    uint64_t mantissa_val = flat_value & double_mantissa_mask;
    mantissa_val <<= sizeof(mantissa_val) * 8 - double_mantissa_size;
    std::memcpy(&mantissa1_,
        reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        sizeof(mantissa1_) + sizeof(mantissa2_));
    mantissa_val <<= (sizeof(mantissa1_) + sizeof(mantissa2_)) * 8;
    mantissa3_ = mantissa_val;
    // handle edge cases (max and min exponent is not a power of 2)
    if ((flat_value & double_exponent_max_mask) == double_exponent_max_mask) {
        exponent_ = quadruple_exponent_max;
        if ((flat_value & single_bit_mask<uint64_t, 0>()) == single_bit_mask<uint64_t, 0>()) {
            exponent_ |= single_bit_mask<uint16_t, 0>();
        }
    } else if ((flat_value | double_exponent_min_mask) == double_exponent_min_mask) {
        exponent_ = quadruple_exponent_min;
        if ((flat_value & single_bit_mask<uint64_t, 0>()) == single_bit_mask<uint64_t, 0>()) {
            exponent_ |= single_bit_mask<uint16_t, 0>();
        }
    } else {
        // remove sing bits
        flat_value <<= 2;
        std::memcpy(&exponent_,
            reinterpret_cast<char*>(&flat_value) + (sizeof(flat_value) - sizeof(exponent_)),
            sizeof(exponent_));
        // align exponent bits
        exponent_ >>= bit_size_of(exponent_) - double_exponent_size + 1;
        // place sing bit
        copy_sign_bits(exponent_, std::bit_cast<uint64_t>(value));
        if (!is_exponent_sing_bit_set(std::bit_cast<uint64_t>(value))) {
            exponent_ |= double_exponent_filler;
        }
    }
}

quadruple::operator float() const noexcept {
    // handle NaN
    if (is_quiet_NaN()) {
        if (signbit()) {
            return -std::numeric_limits<float>::quiet_NaN();
        } else {
            return std::numeric_limits<float>::quiet_NaN();
        }
    } else if (is_signaling_NaN()) {
        if (signbit()) {
            return -std::numeric_limits<float>::signaling_NaN();
        } else {
            return std::numeric_limits<float>::signaling_NaN();
        }
    }

    uint32_t float_bits{0};
    // create exponent
    if ((exponent_ & quadruple_exponent_max) == quadruple_exponent_max) {
        float_bits = float_exponent_max_mask;
        if (signbit()) {
            float_bits |= single_bit_mask<uint32_t, 0>();
        }
    } else if ((exponent_ | single_bit_mask<uint16_t, 0>()) == single_bit_mask<uint16_t, 0>()) {
        float_bits = uint32_t{0};
        if (signbit()) {
            float_bits |= single_bit_mask<uint32_t, 0>();
        }
    } else {
        uint16_t float_exp = exponent_;
        // shift left by extra 2 bits to remove signs
        float_exp <<= bit_size_of(exponent_) - float_exponent_size + 1;
        float_exp >>= 2;
        copy_sign_bits(float_exp, exponent_);
        std::memcpy(reinterpret_cast<char*>(&float_bits) + (sizeof(float_bits) - sizeof(float_exp)),
            &float_exp,
            sizeof(float_exp));
    }

    // create mantissa
    uint64_t mantissa_val{0};
    std::memcpy(reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        &mantissa1_,
        sizeof(mantissa1_) + sizeof(mantissa2_));
    mantissa_val >>= sizeof(mantissa_val) * 8 - float_mantissa_size;

    // combine
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
    // handle NaN
    if (is_quiet_NaN()) {
        if (signbit()) {
            return -std::numeric_limits<double>::quiet_NaN();
        } else {
            return std::numeric_limits<double>::quiet_NaN();
        }
    } else if (is_signaling_NaN()) {
        if (signbit()) {
            return -std::numeric_limits<double>::signaling_NaN();
        } else {
            return std::numeric_limits<double>::signaling_NaN();
        }
    }

    uint64_t double_bits{0};
    // create exponent
    if ((exponent_ & quadruple_exponent_max) == quadruple_exponent_max) {
        double_bits = double_exponent_max_mask;
        if (signbit()) {
            double_bits |= single_bit_mask<uint64_t, 0>();
        }
    } else if ((exponent_ | single_bit_mask<uint16_t, 0>()) == single_bit_mask<uint16_t, 0>()) {
        double_bits = uint64_t{0};
        if (signbit()) {
            double_bits |= single_bit_mask<uint64_t, 0>();
        }
    } else {
        uint16_t double_exp = exponent_;
        // shift left by extra 2 bits to remove signs
        double_exp <<= bit_size_of(exponent_) - double_exponent_size + 1;
        double_exp >>= 2;
        copy_sign_bits(double_exp, exponent_);
        std::memcpy(reinterpret_cast<char*>(&double_bits) + (sizeof(double_bits) - sizeof(double_exp)),
            &double_exp,
            sizeof(double_exp));
    }

    // create mantissa
    uint64_t mantissa_val = mantissa3_;
    mantissa_val >>= (sizeof(mantissa1_) + sizeof(mantissa2_)) * 8;
    std::memcpy(reinterpret_cast<char*>(&mantissa_val) + sizeof(exponent_),
        &mantissa1_,
        sizeof(mantissa1_) + sizeof(mantissa2_));
    mantissa_val >>= sizeof(mantissa_val) * 8 - double_mantissa_size;

    // combine
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

bool quadruple::is_zero() const noexcept {
    static constexpr uint16_t zero_mask = single_bit_mask<uint16_t, 0>();
    return static_cast<uint16_t>(exponent_ | zero_mask) == zero_mask &&
        mantissa1_ == 0 && mantissa2_ == 0 && mantissa3_ == 0;
}

bool quadruple::is_NaN() const noexcept {
    return is_quiet_NaN() || is_signaling_NaN();
}

bool quadruple::is_quiet_NaN() const noexcept {
    return (exponent_ == 0x7FFF || exponent_ == 0xFFFF) &&
           mantissa1_ == 0xFFFF && mantissa2_ == 0xFFFFFFFF && mantissa3_ == 0xFFFFFFFFFFFFFFFF;
}

bool quadruple::is_signaling_NaN() const noexcept {
    return (exponent_ == 0x7FFF || exponent_ == 0xFFFF) &&
           mantissa1_ == 0xAAAA && mantissa2_ == 0xAAAAAAAA && mantissa3_ == 0xAAAAAAAAAAAAAAAA;
}

bool quadruple::is_subnormal() const noexcept {
    static constexpr uint16_t subnormal_mask = single_bit_mask<uint16_t, 0>();
    return static_cast<uint16_t>(exponent_ | subnormal_mask) == subnormal_mask;
}

bool quadruple::signbit() const noexcept {
    return (exponent_ & single_bit_mask<decltype(exponent_), 0>()) != 0;
}

quadruple quadruple::quiet_NaN() noexcept {
    return {0x7FFF,
        0xFFFF,
        0xFFFFFFFF,
        0xFFFFFFFFFFFFFFFF};
}

quadruple quadruple::signaling_NaN() noexcept {
    return {0x7FFF,
            0xAAAA,
            0xAAAAAAAA,
            0xAAAAAAAAAAAAAAAA};
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
    if (is_quiet_NaN() || rhs.is_quiet_NaN()) {
        return quiet_NaN();
    }
    if (is_signaling_NaN() || rhs.is_signaling_NaN()) {
        // TODO: handle exceptions
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
        if (this_sign) {
            return -quadruple{};
        } else {
            return quadruple{};
        }
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
    if (is_quiet_NaN() || rhs.is_quiet_NaN()) {
        return quiet_NaN();
    }
    if (is_signaling_NaN() || rhs.is_signaling_NaN()) {
        // TODO: handle exceptions
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

bool quadruple::operator==(const quadruple& rhs) const noexcept {
    if (is_NaN() || rhs.is_NaN()) {
        return false;
    }
    if (is_zero() && rhs.is_zero()) {
        return true;
    }

    return exponent_ == rhs.exponent_ &&
            mantissa1_ == rhs.mantissa1_ &&
            mantissa2_ == rhs.mantissa2_ &&
            mantissa3_ == rhs.mantissa3_;
}

bool quadruple::operator!=(const quadruple& rhs) const noexcept {
    return !operator==(rhs);
}

bool quadruple::operator<(const quadruple& rhs) const noexcept {
    if (is_NaN() || rhs.is_NaN()) {
        return false;
    }
    auto lhs_sign = signbit();
    auto rhs_sign = rhs.signbit();
    if (lhs_sign != rhs_sign) {
        return lhs_sign > rhs_sign;
    }

    auto lhs_exp = exponent_to_uint16(exponent_);
    auto rhs_exp = exponent_to_uint16(rhs.exponent_);

    if (lhs_exp < rhs_exp) {
        return !lhs_sign;
    } else if (lhs_exp > rhs_exp) {
        return lhs_sign;
    } else {
        return convert_mantissa() < rhs.convert_mantissa();
    }
}

bool quadruple::operator<=(const quadruple& rhs) const noexcept {
    return operator==(rhs) || operator<(rhs);
}

bool quadruple::operator>(const quadruple& rhs) const noexcept {
    return rhs.operator<(*this);
}

bool quadruple::operator>=(const quadruple& rhs) const noexcept {
    return !operator<(rhs);
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
