#pragma once
#include <cstdint>
#include <limits>

#include "utils.hpp"

#ifdef DIMPLICIT_CASTS
#define OPTIONAL_EXPLICIT()
#else
#define OPTIONAL_EXPLICIT() explicit
#endif

// TODO: constexpr

// Designed to fit IEEE 754 standard
class quadruple final{
public:
    quadruple() noexcept = default;
    quadruple(const quadruple&) noexcept = default;
    quadruple(quadruple&&) noexcept = default;
    quadruple& operator=(const quadruple&) noexcept = default;
    quadruple& operator=(quadruple&&) noexcept = default;
    ~quadruple() noexcept = default;

    // Converters
    OPTIONAL_EXPLICIT() quadruple(float value) noexcept;
    OPTIONAL_EXPLICIT() quadruple(double value) noexcept;

    OPTIONAL_EXPLICIT() operator float() const noexcept;
    OPTIONAL_EXPLICIT() operator double() const noexcept;

    // returns true for +0 and -0
    bool is_zero() const noexcept;
    bool is_NaN() const noexcept;
    bool is_quiet_NaN() const noexcept;
    bool is_signaling_NaN() const noexcept;
    bool is_subnormal() const noexcept;
    bool signbit() const noexcept;

    static quadruple quiet_NaN() noexcept;
    static quadruple negative_quiet_NaN() noexcept;
    static quadruple signaling_NaN() noexcept;
    static quadruple negative_signaling_NaN() noexcept;
    static quadruple infinity() noexcept;

    // does not raise FE_INVALID for signaling NaN
    quadruple& flip_sign() noexcept;

    quadruple operator+() const;
    quadruple operator-() const;
    quadruple operator+(const quadruple& rhs) const;
    quadruple operator-(const quadruple& rhs) const;

    bool operator==(const quadruple& rhs) const noexcept;
    bool operator!=(const quadruple& rhs) const noexcept;
    bool operator<(const quadruple& rhs) const noexcept;
    bool operator<=(const quadruple& rhs) const noexcept;
    bool operator>(const quadruple& rhs) const noexcept;
    bool operator>=(const quadruple& rhs) const noexcept;

private:
    // data
    uint16_t exponent_{0};
    uint16_t mantissa1_{0};
    uint32_t mantissa2_{0};
    uint64_t mantissa3_{0};

    // helpers
    quadruple(uint16_t exponent, uint16_t mantissa1, uint32_t mantissa2, uint64_t mantissa3) noexcept;

    struct mantissa_calc {
        uint64_t upper{0};
        uint64_t lower{0};

        bool is_zero() const noexcept;
        [[nodiscard]] int most_significant_bit_position() const noexcept;
        void normalize(int adjustment) noexcept;
        void shift_left(uint32_t amount) noexcept;
        void shift_right(uint32_t amount) noexcept;
        [[nodiscard]] mantissa_calc operator+(const mantissa_calc& rhs) const noexcept;
        [[nodiscard]] mantissa_calc operator-(const mantissa_calc& rhs) const noexcept;

        bool operator<(const mantissa_calc& rhs) const noexcept;
    };

    [[nodiscard]] inline mantissa_calc convert_mantissa() const;

private:
    static constexpr bool UNARY_SIGNALING_PRESERVED_FLOAT = is_sNaN(-std::numeric_limits<float>::signaling_NaN());
    static constexpr bool UNARY_SIGNALING_PRESERVED_DOUBLE = is_sNaN(-std::numeric_limits<double>::signaling_NaN());
    static_assert(UNARY_SIGNALING_PRESERVED_FLOAT == UNARY_SIGNALING_PRESERVED_DOUBLE &&
        "unary signaling behaviour is undefined");

public:
    static constexpr bool UNARY_SIGNALING_PRESERVED =
        UNARY_SIGNALING_PRESERVED_FLOAT == UNARY_SIGNALING_PRESERVED_DOUBLE ? UNARY_SIGNALING_PRESERVED_FLOAT : false;
};