#pragma once
#include <cstdint>
#include <limits>

#include "utils.hpp"

// TODO: constexpr

// Designed to fit IEEE 754 standard
class quadruple final {
public:
    constexpr quadruple() noexcept = default;
    constexpr quadruple(const quadruple&) noexcept = default;
    constexpr quadruple(quadruple&&) noexcept = default;
    constexpr quadruple& operator=(const quadruple&) noexcept = default;
    constexpr quadruple& operator=(quadruple&&) noexcept = default;
    constexpr ~quadruple() noexcept = default;

    template <typename T>
    requires std::is_integral_v<T>
    OPTIONAL_EXPLICIT()
    quadruple(T value) noexcept;
    OPTIONAL_EXPLICIT() quadruple(uint64_t value) noexcept;

    OPTIONAL_EXPLICIT() quadruple(float value) noexcept;
    OPTIONAL_EXPLICIT() quadruple(double value) noexcept;

#if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)
    static_assert(__SIZEOF_INT128__ == 16);

    OPTIONAL_EXPLICIT() quadruple(__int128 value) noexcept;
    OPTIONAL_EXPLICIT() quadruple(unsigned __int128 value) noexcept;

    OPTIONAL_EXPLICIT() operator __int128() const noexcept;
    OPTIONAL_EXPLICIT() operator unsigned __int128() const noexcept;
#endif

    OPTIONAL_EXPLICIT() operator int8_t() const noexcept;
    OPTIONAL_EXPLICIT() operator int16_t() const noexcept;
    OPTIONAL_EXPLICIT() operator int32_t() const noexcept;
    OPTIONAL_EXPLICIT() operator int64_t() const noexcept;
    OPTIONAL_EXPLICIT() operator uint8_t() const noexcept;
    OPTIONAL_EXPLICIT() operator uint16_t() const noexcept;
    OPTIONAL_EXPLICIT() operator uint32_t() const noexcept;
    OPTIONAL_EXPLICIT() operator uint64_t() const noexcept;

    OPTIONAL_EXPLICIT() operator float() const noexcept;
    OPTIONAL_EXPLICIT() operator double() const noexcept;

    // returns true for +0 and -0
    constexpr bool is_zero() const noexcept;
    constexpr bool is_NaN() const noexcept;
    constexpr bool is_quiet_NaN() const noexcept;
    constexpr bool is_signaling_NaN() const noexcept;
    constexpr bool is_subnormal() const noexcept;
    constexpr bool signbit() const noexcept;

    // TODO: NaN packing
    static constexpr quadruple quiet_NaN() noexcept;
    static constexpr quadruple negative_quiet_NaN() noexcept;
    static constexpr quadruple signaling_NaN() noexcept;
    static constexpr quadruple negative_signaling_NaN() noexcept;
    static constexpr quadruple infinity() noexcept;
    static constexpr quadruple negative_infinity() noexcept;
    static constexpr quadruple max() noexcept;
    static constexpr quadruple min() noexcept;

    // does not raise FE_INVALID for signaling NaN
    constexpr quadruple& flip_sign() noexcept;

    quadruple operator+() const;
    quadruple operator-() const;
    quadruple operator+(const quadruple& rhs) const;
    quadruple operator-(const quadruple& rhs) const;
    quadruple operator*(const quadruple& rhs) const;
    quadruple& operator+=(const quadruple& rhs);
    quadruple& operator-=(const quadruple& rhs);
    quadruple& operator*=(const quadruple& rhs);

    bool operator==(const quadruple& rhs) const noexcept;
    bool operator!=(const quadruple& rhs) const noexcept;
    bool operator<(const quadruple& rhs) const noexcept;
    bool operator<=(const quadruple& rhs) const noexcept;
    bool operator>(const quadruple& rhs) const noexcept;
    bool operator>=(const quadruple& rhs) const noexcept;
    std::partial_ordering operator<=>(const quadruple& rhs) const noexcept;

private:
    // data
    uint64_t lower_{0};
    uint64_t upper_{0};

    // helpers
    constexpr quadruple(uint64_t upper, uint64_t lower) noexcept;

    struct mantissa_calc {
        uint64_t lower{0};
        uint64_t upper{0};

        bool is_zero() const noexcept;
        [[nodiscard]] int most_significant_bit_position() const noexcept;
        void normalize(int adjustment) noexcept;
        void shift_left(uint32_t amount) noexcept;
        void shift_right(uint32_t amount) noexcept;
        [[nodiscard]] mantissa_calc operator+(const mantissa_calc& rhs) const noexcept;
        [[nodiscard]] mantissa_calc operator-(const mantissa_calc& rhs) const noexcept;
        [[nodiscard]] mantissa_calc operator*(const mantissa_calc& rhs) const noexcept;

        bool operator<(const mantissa_calc& rhs) const noexcept;
    };

    [[nodiscard]] inline mantissa_calc convert_mantissa() const;
};

constexpr quadruple::quadruple(uint64_t upper, uint64_t lower) noexcept
    : lower_(lower)
    , upper_(upper) {}

constexpr bool quadruple::is_zero() const noexcept { return (upper_ | zero_mask) == zero_mask && lower_ == 0; }
constexpr bool quadruple::is_NaN() const noexcept { return is_quiet_NaN() || is_signaling_NaN(); }
constexpr bool quadruple::is_quiet_NaN() const noexcept {
    return (upper_ == 0x7FFFFFFFFFFFFFFF || upper_ == 0xFFFFFFFFFFFFFFFF) && lower_ == 0xFFFFFFFFFFFFFFFF;
}
constexpr bool quadruple::is_signaling_NaN() const noexcept {
    return (upper_ == 0x7FFFAAAAAAAAAAAA || upper_ == 0xFFFFAAAAAAAAAAAA) && lower_ == 0xAAAAAAAAAAAAAAAA;
}
constexpr bool quadruple::is_subnormal() const noexcept {
    return (upper_ & subnormal_mask) == 0 && ((upper_ & upper_mantissa_mask) != 0 || lower_ != 0);
}
constexpr bool quadruple::signbit() const noexcept { return (upper_ & sign_bit_mask) != 0; }

constexpr quadruple quadruple::quiet_NaN() noexcept { return {0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}; }
constexpr quadruple quadruple::negative_quiet_NaN() noexcept { return {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}; }
constexpr quadruple quadruple::signaling_NaN() noexcept { return {0x7FFFAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA}; }
constexpr quadruple quadruple::negative_signaling_NaN() noexcept { return {0xFFFFAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA}; }

constexpr quadruple quadruple::infinity() noexcept { return {0x7FFF000000000000, 0}; }
constexpr quadruple quadruple::negative_infinity() noexcept { return {0xFFFF000000000000, 0}; }

constexpr quadruple quadruple::max() noexcept { return {0x7FFEFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}; }
constexpr quadruple quadruple::min() noexcept { return {0x0001000000000000, 0}; }

constexpr quadruple& quadruple::flip_sign() noexcept {
    upper_ ^= sign_bit_mask;
    return *this;
}

template <typename T>
requires std::is_integral_v<T>
quadruple::quadruple(T value) noexcept
    : quadruple(value < 0 ? static_cast<uint64_t>(-value) : static_cast<uint64_t>(value)) {
    if (value < 0) {
        flip_sign();
    }
}
