#pragma once
#include <cstdint>

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

private:
    uint16_t exponent_{0};
    uint16_t mantissa1_{0};
    uint32_t mantissa2_{0};
    uint64_t mantissa3_{0};
};