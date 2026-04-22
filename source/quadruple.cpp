#include "quadruple.hpp"
#include "ub_consistency.hpp"
#include "utils.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <stdexcept>

static_assert(sizeof(quadruple) == 16);
static_assert(alignof(quadruple) == 8);
static_assert(std::is_default_constructible_v<quadruple>);
static_assert(std::is_trivially_destructible_v<quadruple>);
static_assert(std::is_trivially_copyable_v<quadruple>);
static_assert(std::is_trivially_copy_assignable_v<quadruple>);
static_assert(std::is_trivially_move_constructible_v<quadruple>);
static_assert(std::is_trivially_move_assignable_v<quadruple>);

quadruple::quadruple(uint64_t value) noexcept {
    if (value == 0) {
        return;
    }

    auto msb = most_significant_bit_position<uint64_t>(value);
    uint64_t exponent_val = exponent_values::quadruple_exponent_bias +
                            static_cast<uint16_t>(sizeof(uint64_t) * 8 - static_cast<uint64_t>(msb) - 1);
    exponent_val <<= (sizeof(uint64_t) - sizeof(uint16_t)) * 8;
    assert((exponent_val & upper_mantissa_mask) == 0);

    if (msb > static_cast<int>(quadruple_exponent_size)) {
        uint64_t mantissa_val = value << (msb - static_cast<int>(quadruple_exponent_size));
        mantissa_val &= upper_mantissa_mask;
        upper_ = exponent_val | mantissa_val;
    } else {
        uint64_t mantissa_val1 = value >> (static_cast<int>(quadruple_exponent_size) - msb);
        mantissa_val1 &= upper_mantissa_mask;
        uint64_t mantissa_val2 = value << (static_cast<int>(sizeof(uint64_t) * 8 - quadruple_exponent_size) + msb);
        upper_ = exponent_val | mantissa_val1;
        lower_ = mantissa_val2;
    }
}

quadruple::quadruple(float value) noexcept {
    // handle NaN
    if (is_qNaN(value)) {
        if (std::signbit(value)) {
            *this = negative_quiet_NaN();
        } else {
            *this = quiet_NaN();
        }
        return;
    } else if (is_sNaN(value)) {
        if (std::signbit(value)) {
            *this = negative_signaling_NaN();
        } else {
            *this = signaling_NaN();
        }
        return;
    }

    auto flat_value = std::bit_cast<uint32_t>(value);
    uint64_t exponent_val{0};
    uint64_t mantissa_val = static_cast<uint64_t>(flat_value) & float_mantissa_mask;
    mantissa_val <<= sizeof(mantissa_val) * 8 - (float_mantissa_size + quadruple_exponent_size + 1);

    // handle edge cases (max and min exponent is not a power of 2)
    if ((flat_value & float_exponent_max_mask) == float_exponent_max_mask) {
        exponent_val = quadruple_exponent_max;
        if ((flat_value & single_bit_mask<uint32_t, 0>()) == single_bit_mask<uint32_t, 0>()) {
            exponent_val |= sign_bit_mask;
        }
    } else if ((flat_value | float_exponent_min_mask) == float_exponent_min_mask) {
        if (mantissa_val == 0) {
            exponent_val = quadruple_exponent_min;
            if ((flat_value & single_bit_mask<uint32_t, 0>()) == single_bit_mask<uint32_t, 0>()) {
                exponent_val |= sign_bit_mask;
            }
        } else {
            mantissa_calc mantissa{0, mantissa_val};
            // we can represent that value using a normal numbers, but we need some convergence
            exponent_val = float_subnormal_exponent_filler;
            if (std::signbit(value)) {
                exponent_val |= sign_bit_mask;
            }
            // find how much we have to shift mantissa and adjust exponent
            auto msb = mantissa.most_significant_bit_position();
            int exponent_adj = static_cast<int>(quadruple_exponent_size) - msb;
            exponent_val -= static_cast<uint64_t>(-exponent_adj) << ((sizeof(uint64_t) - sizeof(uint16_t)) * 8);
            // convert mantissa back
            mantissa.normalize(exponent_adj);
            // make first '1' implied, since value in quadruple is normal
            mantissa_val = mantissa.upper & upper_mantissa_mask;
        }
    } else {
        exponent_val = static_cast<uint64_t>(flat_value) & ~float_mantissa_mask;
        exponent_val <<= (sizeof(exponent_val) - sizeof(flat_value)) * 8;
        // store sign and exponent sign for later
        uint64_t sign_bits = exponent_val & sign_bits_mask;
        exponent_val <<= 2;
        // align exponent bits
        exponent_val >>= quadruple_exponent_size - float_exponent_size + 2;
        // place sign bit
        exponent_val |= sign_bits;
        if (!is_exponent_sign_bit_set(std::bit_cast<uint32_t>(value))) {
            exponent_val |= float_exponent_filler;
        }
    }

    assert((exponent_val & upper_mantissa_mask) == 0);
    assert((mantissa_val & upper_mantissa_mask) == mantissa_val);
    upper_ = exponent_val | mantissa_val;
}

quadruple::quadruple(double value) noexcept {
    // handle NaN
    if (is_qNaN(value)) {
        if (std::signbit(value)) {
            *this = negative_quiet_NaN();
        } else {
            *this = quiet_NaN();
        }
        return;
    } else if (is_sNaN(value)) {
        if (std::signbit(value)) {
            *this = negative_signaling_NaN();
        } else {
            *this = signaling_NaN();
        }
        return;
    }

    auto flat_value = std::bit_cast<uint64_t>(value);
    uint64_t exponent_val{0};
    uint64_t mantissa_val1 = flat_value & double_mantissa_mask;
    uint64_t mantissa_val2 = mantissa_val1;
    mantissa_val1 >>= quadruple_exponent_size - double_exponent_size;
    mantissa_val2 <<= sizeof(uint64_t) * 8 - (quadruple_exponent_size - double_exponent_size);

    // handle edge cases (max and min exponent is not a power of 2)
    if ((flat_value & double_exponent_max_mask) == double_exponent_max_mask) {
        exponent_val = quadruple_exponent_max;
        if ((flat_value & sign_bit_mask) == sign_bit_mask) {
            exponent_val |= sign_bit_mask;
        }
    } else if ((flat_value | double_exponent_min_mask) == double_exponent_min_mask) {
        if (mantissa_val1 == 0 && mantissa_val2 == 0) {
            exponent_val = quadruple_exponent_min;
            if ((flat_value & sign_bit_mask) == sign_bit_mask) {
                exponent_val |= sign_bit_mask;
            }
        } else {
            mantissa_calc mantissa{mantissa_val2, mantissa_val1};
            // we can represent that value using a normal numbers, but we need some convergence
            exponent_val = double_subnormal_exponent_filler;
            if (std::signbit(value)) {
                exponent_val |= sign_bit_mask;
            }
            // find how much we have to shift mantissa and adjust exponent
            auto msb = mantissa.most_significant_bit_position();
            int exponent_adj = static_cast<int>(quadruple_exponent_size) - msb;
            exponent_val -= static_cast<uint64_t>(-exponent_adj) << ((sizeof(uint64_t) - sizeof(uint16_t)) * 8);
            // convert mantissa back
            mantissa.normalize(exponent_adj);
            // make first '1' implied, since value in quadruple is normal
            mantissa_val1 = mantissa.upper & upper_mantissa_mask;
            mantissa_val2 = mantissa.lower;
        }
    } else {
        exponent_val = flat_value & ~double_mantissa_mask;
        // store sign and exponent sign for later
        uint64_t sign_bits = exponent_val & sign_bits_mask;
        exponent_val <<= 2;
        // align exponent bits
        exponent_val >>= quadruple_exponent_size - double_exponent_size + 2;
        // place sign bit
        exponent_val |= sign_bits;
        if (!is_exponent_sign_bit_set(std::bit_cast<uint64_t>(value))) {
            exponent_val |= double_exponent_filler;
        }
    }

    assert((exponent_val & upper_mantissa_mask) == 0);
    assert((mantissa_val1 & upper_mantissa_mask) == mantissa_val1);
    upper_ = exponent_val | mantissa_val1;
    lower_ = mantissa_val2;
}

#if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)

quadruple::quadruple(__int128 value) noexcept
    : quadruple(value < 0 ? static_cast<unsigned __int128>(-value) : static_cast<unsigned __int128>(value)) {
    if (value < 0) {
        flip_sign();
    }
}

quadruple::quadruple(unsigned __int128 value) noexcept {
    if (value == 0) {
        return;
    }
    mantissa_calc split_value = std::bit_cast<mantissa_calc>(value);

    auto msb = split_value.most_significant_bit_position();
    uint64_t exponent_val = exponent_values::quadruple_exponent_bias +
                            static_cast<uint16_t>(sizeof(unsigned __int128) * 8 - static_cast<uint64_t>(msb) - 1);
    exponent_val <<= (sizeof(uint64_t) - sizeof(uint16_t)) * 8;
    assert((exponent_val & upper_mantissa_mask) == 0);

    auto shift = static_cast<int>(quadruple_exponent_size) - msb;
    split_value.normalize(shift);
    lower_ = split_value.lower;
    upper_ = (split_value.upper & ~implied_bit_mask) | exponent_val;
}

quadruple::operator __int128() const noexcept {
    if ((upper_ & ~sign_bit_mask & ~upper_mantissa_mask) == quadruple_exponent_max) {
        // NaN or Inf
        std::feraiseexcept(FE_INVALID);
        if (is_NaN()) {
            return (__int128{0x8000000000000000} << 64) + __int128{0x8000000000000000};
        } else {
            return __int128{0};
        }
    }
    if ((upper_ & ~upper_mantissa_mask) < quadruple_exponent_bias) {
        return 0;
    } else {
        uint64_t result_bit_size = upper_ & ~sign_bit_mask & ~upper_mantissa_mask;
        if (result_bit_size < quadruple_exponent_bias) {
            return 0;
        }
        result_bit_size -= quadruple_exponent_bias;
        result_bit_size = (result_bit_size >> (sizeof(uint64_t) - sizeof(uint16_t)) * 8) + 1;
        if (result_bit_size > sizeof(__int128) * 8) {
            return 0;
        }
        mantissa_calc result_mantissa = convert_mantissa();
        auto shift = static_cast<int>(sizeof(mantissa_calc) * 8 - quadruple_exponent_size - result_bit_size);
        result_mantissa.normalize(shift);
        if (signbit()) {
            return -static_cast<__int128>(std::bit_cast<unsigned __int128>(result_mantissa));
        } else {
            return static_cast<__int128>(std::bit_cast<unsigned __int128>(result_mantissa));
        }
    }
}

quadruple::operator unsigned __int128() const noexcept {
    if ((upper_ & ~sign_bit_mask & ~upper_mantissa_mask) == quadruple_exponent_max) {
        // NaN or Inf
        std::feraiseexcept(FE_INVALID);
        if (is_NaN()) {
            return static_cast<unsigned __int128>((__int128{0x8000000000000000} << 64) + __int128{0x8000000000000000});
        } else {
            if (signbit()) {
                return static_cast<unsigned __int128>((__int128{0x8000000000000000} << 64) +
                                                      __int128{0x8000000000000000});
            } else {
                return (unsigned __int128){0};
            }
        }
    }
    if ((upper_ & ~upper_mantissa_mask) < quadruple_exponent_bias) {
        return 0;
    } else {
        uint64_t result_bit_size = upper_ & ~sign_bit_mask & ~upper_mantissa_mask;
        if (result_bit_size < quadruple_exponent_bias) {
            return 0;
        }
        result_bit_size -= quadruple_exponent_bias;
        result_bit_size = (result_bit_size >> (sizeof(uint64_t) - sizeof(uint16_t)) * 8) + 1;
        if (result_bit_size > sizeof(unsigned __int128) * 8) {
            if (signbit()) {
                return ((unsigned __int128){0x8000000000000000} << 64) + (unsigned __int128){0x8000000000000000};
            } else {
                return 0;
            }
        }
        mantissa_calc result_mantissa = convert_mantissa();
        auto shift = static_cast<int>(sizeof(mantissa_calc) * 8 - quadruple_exponent_size - result_bit_size);
        result_mantissa.normalize(shift);
        if (signbit() && result_bit_size >= sizeof(uint64_t) * 8) {
            result_mantissa.upper = std::numeric_limits<uint64_t>::max() - result_mantissa.upper + 1;
            result_mantissa.lower = 0x8000000000000000;
            return std::bit_cast<unsigned __int128>(result_mantissa);
        }
        auto res = std::bit_cast<unsigned __int128>(result_mantissa);
        if (signbit()) {
            return (static_cast<unsigned __int128>(std::numeric_limits<uint64_t>::max()) + (unsigned __int128){1}) -
                   res;
        } else {
            return std::bit_cast<unsigned __int128>(result_mantissa);
        }
    }
}

#endif

quadruple::operator int8_t() const noexcept { return static_cast<int8_t>(static_cast<int32_t>(*this)); }

quadruple::operator int16_t() const noexcept { return static_cast<int16_t>(static_cast<int32_t>(*this)); }

quadruple::operator int32_t() const noexcept {
    if ((upper_ & ~sign_bit_mask & ~upper_mantissa_mask) == quadruple_exponent_max) {
        // NaN or Inf
        std::feraiseexcept(FE_INVALID);
        return UB_handle::to_integer_conversion::NEG_OVERFLOW<int32_t>;
    }
    if ((upper_ & ~upper_mantissa_mask) < quadruple_exponent_bias) {
        return 0;
    } else {
        uint64_t result;
        uint64_t result_bit_size = upper_ & ~sign_bit_mask & ~upper_mantissa_mask;
        if (result_bit_size < quadruple_exponent_bias) {
            return 0;
        }
        result_bit_size -= quadruple_exponent_bias;
        result_bit_size = (result_bit_size >> (sizeof(uint64_t) - sizeof(uint16_t)) * 8) + 1;
        if (result_bit_size >= sizeof(int32_t) * 8) {
            return UB_handle::to_integer_conversion::NEG_OVERFLOW<int32_t>;
        }
        mantissa_calc result_mantissa = convert_mantissa();
        auto shift = static_cast<int>(sizeof(mantissa_calc) * 8 - quadruple_exponent_size - result_bit_size);
        result_mantissa.normalize(shift);
        result = result_mantissa.lower;
        if (signbit()) {
            return -static_cast<int32_t>(result);
        } else {
            return static_cast<int32_t>(result);
        }
    }
}

quadruple::operator int64_t() const noexcept {
    if ((upper_ & ~sign_bit_mask & ~upper_mantissa_mask) == quadruple_exponent_max) {
        // NaN or Inf
        std::feraiseexcept(FE_INVALID);
        return UB_handle::to_integer_conversion::NEG_OVERFLOW<int64_t>;
    }
    if ((upper_ & ~upper_mantissa_mask) < quadruple_exponent_bias) {
        return 0;
    } else {
        uint64_t result;
        uint64_t result_bit_size = upper_ & ~sign_bit_mask & ~upper_mantissa_mask;
        if (result_bit_size < quadruple_exponent_bias) {
            return 0;
        }
        result_bit_size -= quadruple_exponent_bias;
        result_bit_size = (result_bit_size >> (sizeof(uint64_t) - sizeof(uint16_t)) * 8) + 1;
        if (result_bit_size >= sizeof(int64_t) * 8) {
            return UB_handle::to_integer_conversion::NEG_OVERFLOW<int64_t>;
        }
        mantissa_calc result_mantissa = convert_mantissa();
        auto shift = static_cast<int>(sizeof(mantissa_calc) * 8 - quadruple_exponent_size - result_bit_size);
        result_mantissa.normalize(shift);
        result = result_mantissa.lower;
        if (signbit()) {
            return -static_cast<int64_t>(result);
        } else {
            return static_cast<int64_t>(result);
        }
    }
}

quadruple::operator uint8_t() const noexcept { return static_cast<uint8_t>(static_cast<int32_t>(*this)); }

quadruple::operator uint16_t() const noexcept { return static_cast<uint16_t>(static_cast<int32_t>(*this)); }

quadruple::operator uint32_t() const noexcept {
    if ((upper_ & ~sign_bit_mask & ~upper_mantissa_mask) == quadruple_exponent_max) {
        // NaN or Inf
        std::feraiseexcept(FE_INVALID);
        return UB_handle::to_integer_conversion::NEG_OVERFLOW<uint32_t>;
    }
    if ((upper_ & ~upper_mantissa_mask) < quadruple_exponent_bias) {
        return 0;
    } else {
        uint64_t result;
        uint64_t result_bit_size = upper_ & ~sign_bit_mask & ~upper_mantissa_mask;
        if (result_bit_size < quadruple_exponent_bias) {
            return 0;
        }
        result_bit_size -= quadruple_exponent_bias;
        result_bit_size = (result_bit_size >> (sizeof(uint64_t) - sizeof(uint16_t)) * 8) + 1;
        if (result_bit_size >= sizeof(uint64_t) * 8) {
            return UB_handle::to_integer_conversion::NaN<uint32_t>;
        }
        mantissa_calc result_mantissa = convert_mantissa();
        auto shift = static_cast<int>(sizeof(mantissa_calc) * 8 - quadruple_exponent_size - result_bit_size);
        result_mantissa.normalize(shift);
        result = result_mantissa.lower;
        if (signbit()) {
            return std::numeric_limits<uint32_t>::max() - static_cast<uint32_t>(result) + 1;
        } else {
            return static_cast<uint32_t>(result);
        }
    }
}

quadruple::operator uint64_t() const noexcept {
    if ((upper_ & ~sign_bit_mask & ~upper_mantissa_mask) == quadruple_exponent_max) {
        // NaN or Inf
        std::feraiseexcept(FE_INVALID);
#if defined(NDEBUG) && defined(__OPTIMIZE__)
        if (is_NaN()) {
            return static_cast<uint64_t>(UB_handle::to_integer_conversion::NEG_OVERFLOW<int64_t>);
        } else {
            if (signbit()) {
                return static_cast<uint64_t>(UB_handle::to_integer_conversion::NEG_OVERFLOW<int64_t>);
            } else {
                return UB_handle::to_integer_conversion::NEG_OVERFLOW<uint64_t>;
            }
        }
#else
        if (is_NaN()) {
            return UB_handle::to_integer_conversion::NaN<uint64_t>;
        } else {
            if (signbit()) {
                return UB_handle::to_integer_conversion::NEG_OVERFLOW<uint64_t>;
            } else {
                return UB_handle::to_integer_conversion::POS_OVERFLOW<uint64_t>;
            }
        }
#endif
    }
    if ((upper_ & ~upper_mantissa_mask) < quadruple_exponent_bias) {
        return 0;
    } else {
        uint64_t result;
        uint64_t result_bit_size = upper_ & ~sign_bit_mask & ~upper_mantissa_mask;
        if (result_bit_size < quadruple_exponent_bias) {
            return 0;
        }
        result_bit_size -= quadruple_exponent_bias;
        result_bit_size = (result_bit_size >> (sizeof(uint64_t) - sizeof(uint16_t)) * 8) + 1;
        if (result_bit_size + static_cast<uint64_t>(signbit()) > sizeof(uint64_t) * 8) {
            if (signbit()) {
                return 0x8000000000000000;
            } else {
                return 0;
            }
        }
        mantissa_calc result_mantissa = convert_mantissa();
        auto shift = static_cast<int>(sizeof(mantissa_calc) * 8 - quadruple_exponent_size - result_bit_size);
        result_mantissa.normalize(shift);
        result = result_mantissa.lower;
        if (signbit()) {
            return std::numeric_limits<uint64_t>::max() - result + 1;
        } else {
            return result;
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
            return negative_sNaN<float>();
        } else {
            return std::numeric_limits<float>::signaling_NaN();
        }
    }

    uint32_t float_bits{0};

    // create mantissa
    uint64_t exponent_val = upper_ & ~upper_mantissa_mask;
    uint64_t mantissa_val = upper_ & upper_mantissa_mask;
    mantissa_val >>= (sizeof(uint64_t) - sizeof(uint16_t)) * 8 - float_mantissa_size;

    // create exponent
    bool forced_round = false;
    bool requires_round = false;
    if ((exponent_val & quadruple_exponent_max) == quadruple_exponent_max) {
        float_bits = float_exponent_max_mask;
        if (signbit()) {
            float_bits |= single_bit_mask<uint32_t, 0>();
        }
    } else {
        auto numeric_exponent = exponent_to_uint16(exponent_val);
        if (numeric_exponent >= exponent_values::max_float_exponent) {
            return signbit() ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
        } else if (numeric_exponent <= exponent_values::min_float_exponent) {
            if (signbit()) {
                float_bits |= single_bit_mask<uint32_t, 0>();
            }
            forced_round = true;
            size_t adj = exponent_values::min_float_exponent - numeric_exponent + 1;
            if (adj == float_mantissa_size + 1) {
                mantissa_val = 1;
            } else {
                if (adj > float_mantissa_size + 1) {
                    mantissa_val = 0;
                } else {
                    if (adj > 0) {
                        mantissa_val >>= adj - 1;
                        requires_round =
                            (mantissa_val & single_bit_mask<uint64_t, 63>()) == single_bit_mask<uint64_t, 63>();
                        mantissa_val >>= 1;
                    }
                }
                if (adj <= float_mantissa_size) {
                    mantissa_val |= uint64_t{1} << (float_mantissa_size - adj);
                }
            }
        } else {
            // shift left by extra 2 bits to remove signs
            exponent_val <<= quadruple_exponent_size - float_exponent_size + 2;
            exponent_val >>= 2;
            exponent_val |= (upper_ & sign_bits_mask);
            std::memcpy(&float_bits,
                        reinterpret_cast<char*>(&exponent_val) + sizeof(exponent_val) - sizeof(float_bits),
                        sizeof(float_bits));
        }
    }

    // combine
    float_bits |= static_cast<uint32_t>(mantissa_val);

    auto result = std::bit_cast<float>(float_bits);
    if ((forced_round && requires_round) ||
        (!forced_round && is_bit_set<uint64_t, quadruple_exponent_size + float_mantissa_size + 1>(upper_) &&
         (float_bits | single_bit_mask<uint32_t, 0>()) != single_bit_mask<uint32_t, 0>())) {
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
            return negative_sNaN<double>();
        } else {
            return std::numeric_limits<double>::signaling_NaN();
        }
    }

    uint64_t double_bits{0};

    // create mantissa
    uint64_t exponent_val = upper_ & ~upper_mantissa_mask;
    uint64_t mantissa_val = upper_ & upper_mantissa_mask;
    mantissa_val <<= quadruple_exponent_size - double_exponent_size;
    mantissa_val |= lower_ >> (sizeof(lower_) * 8 - (quadruple_exponent_size - double_exponent_size));

    // create exponent
    bool forced_round = false;
    bool requires_round = false;
    if ((exponent_val & quadruple_exponent_max) == quadruple_exponent_max) {
        double_bits = double_exponent_max_mask;
        if (signbit()) {
            double_bits |= sign_bit_mask;
        }
    } else {
        auto numeric_exponent = exponent_to_uint16(exponent_val);
        if (numeric_exponent >= exponent_values::max_double_exponent) {
            return signbit() ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
        } else if (numeric_exponent <= exponent_values::min_double_exponent) {
            if (signbit()) {
                double_bits |= sign_bit_mask;
            }
            forced_round = true;
            size_t adj = exponent_values::min_double_exponent - numeric_exponent + 1;
            if (adj == double_mantissa_size + 1) {
                mantissa_val = 1;
            } else {
                if (adj > double_mantissa_size + 1) {
                    mantissa_val = 0;
                } else {
                    if (adj > 0) {
                        mantissa_val >>= adj - 1;
                        requires_round =
                            (mantissa_val & single_bit_mask<uint64_t, 63>()) == single_bit_mask<uint64_t, 63>();
                        mantissa_val >>= 1;
                    }
                }
                if (adj <= double_mantissa_size) {
                    mantissa_val |= uint64_t{1} << (double_mantissa_size - adj);
                }
            }
        } else {
            // shift left by extra 2 bits to remove signs
            exponent_val <<= quadruple_exponent_size - double_exponent_size + 2;
            exponent_val >>= 2;
            exponent_val |= (upper_ & sign_bits_mask);
            std::memcpy(&double_bits, &exponent_val, sizeof(double_bits));
        }
    }

    // combine
    double_bits |= mantissa_val;

    auto result = std::bit_cast<double>(double_bits);
    if ((forced_round && requires_round) ||
        (!forced_round && is_bit_set<decltype(lower_), quadruple_exponent_size - double_exponent_size>(lower_) &&
         (double_bits | sign_bit_mask) != sign_bit_mask)) {
        if (is_bit_set<decltype(double_bits), 0>(double_bits)) {
            return std::nextafter(result, -std::numeric_limits<double>::infinity());
        } else {
            return std::nextafter(result, std::numeric_limits<double>::infinity());
        }
    } else {
        return result;
    }
}

quadruple quadruple::operator+() const {
    if constexpr (UB_handle::unary_signaling::PRESERVED) {
        return *this;
    } else {
        if (is_signaling_NaN()) {
            std::feraiseexcept(FE_INVALID);
            if (signbit()) {
                return negative_quiet_NaN();
            } else {
                return quiet_NaN();
            }
        } else {
            return *this;
        }
    }
}

quadruple quadruple::operator-() const {
    if constexpr (UB_handle::unary_signaling::PRESERVED) {
        return {upper_ ^ sign_bit_mask, lower_};
    } else {
        if (is_signaling_NaN()) {
            std::feraiseexcept(FE_INVALID);
            if (signbit()) {
                return quiet_NaN();
            } else {
                return negative_quiet_NaN();
            }
        } else {
            return {upper_ ^ sign_bit_mask, lower_};
        }
    }
}

quadruple quadruple::operator+(const quadruple& rhs) const {
    if (is_signaling_NaN() || rhs.is_signaling_NaN()) {
        std::feraiseexcept(FE_INVALID);
        if (signbit()) {
            return negative_quiet_NaN();
        } else {
            return quiet_NaN();
        }
    }
    if (is_quiet_NaN() || rhs.is_quiet_NaN()) {
        if (signbit()) {
            return negative_quiet_NaN();
        } else {
            return quiet_NaN();
        }
    }

    auto this_sign = signbit();
    if (this_sign != rhs.signbit()) {
        if (this_sign) {
            return rhs - -(*this);
        } else {
            return operator-(-rhs);
        }
    }
    // handle infinity
    if ((upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
        return *this;
    } else if ((rhs.upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
        return rhs;
    }

    auto lhs_mantissa = convert_mantissa();
    auto rhs_mantissa = rhs.convert_mantissa();
    auto exp_diff = exponent_difference(upper_, rhs.upper_);

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
    int exponent_adj = static_cast<int>(quadruple_exponent_size) - msb;
    auto adjusted_exponent = static_cast<uint64_t>(exponent_to_uint16(upper_) + static_cast<int16_t>(exponent_adj));
    if (adjusted_exponent == std::numeric_limits<uint64_t>::max()) {
        auto res = quadruple{res_mantissa.upper, res_mantissa.lower};
        if (this_sign) {
            res.upper_ |= sign_bit_mask;
        }
        return res;
    }
    adjusted_exponent <<= (sizeof(uint64_t) - sizeof(uint16_t)) * 8;
    if ((adjusted_exponent & sign_bit_mask) != 0) {
        // exponent overflow
        if (this_sign) {
            return -infinity();
        } else {
            return infinity();
        }
    }
    res_mantissa.normalize(exponent_adj);
    if (adjusted_exponent != 0) {
        res_mantissa.upper &= upper_mantissa_mask;
    }

    auto res = quadruple{adjusted_exponent | res_mantissa.upper, res_mantissa.lower};
    if (this_sign) {
        res.upper_ |= sign_bit_mask;
    }
    return res;
}

quadruple quadruple::operator-(const quadruple& rhs) const {
    if (is_signaling_NaN() || rhs.is_signaling_NaN()) {
        std::feraiseexcept(FE_INVALID);
        if (signbit()) {
            return negative_quiet_NaN();
        } else {
            return quiet_NaN();
        }
    }
    if (is_quiet_NaN() || rhs.is_quiet_NaN()) {
        if (signbit()) {
            return negative_quiet_NaN();
        } else {
            return quiet_NaN();
        }
    }

    auto this_sign = signbit();
    if (this_sign != rhs.signbit()) {
        return operator+(-rhs);
    }
    // handle infinity
    if ((upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
        if ((rhs.upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
            return negative_quiet_NaN();
        } else {
            return *this;
        }
    } else if ((rhs.upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
        return rhs;
    }

    auto lhs_mantissa = convert_mantissa();
    auto rhs_mantissa = rhs.convert_mantissa();
    auto exp_diff = exponent_difference(upper_, rhs.upper_);

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
    int exponent_adj = static_cast<int>(quadruple_exponent_size) - msb;
    uint64_t adjusted_exponent;
    if (flipped_sign) {
        adjusted_exponent = static_cast<uint64_t>(exponent_to_uint16(rhs.upper_) + static_cast<int16_t>(exponent_adj));
    } else {
        adjusted_exponent = static_cast<uint64_t>(exponent_to_uint16(upper_) + static_cast<int16_t>(exponent_adj));
    }
    if (adjusted_exponent == std::numeric_limits<uint64_t>::max()) {
        auto res = quadruple{res_mantissa.upper, res_mantissa.lower};
        if (this_sign) {
            res.upper_ |= sign_bit_mask;
        }
        return res;
    }
    adjusted_exponent <<= (sizeof(uint64_t) - sizeof(uint16_t)) * 8;
    if ((adjusted_exponent & sign_bit_mask) != 0) {
        // exponent overflow
        if (this_sign) {
            return -infinity();
        } else {
            return infinity();
        }
    }
    res_mantissa.normalize(exponent_adj);
    res_mantissa.upper &= upper_mantissa_mask;

    auto res = quadruple{adjusted_exponent | res_mantissa.upper, res_mantissa.lower};
    if (this_sign) {
        res.upper_ |= sign_bit_mask;
    }
    return res;
}

quadruple quadruple::operator*(const quadruple& rhs) const {
    bool result_sign = signbit() != rhs.signbit();

    if (is_signaling_NaN()) {
        std::feraiseexcept(FE_INVALID);
        if (signbit()) {
            return negative_quiet_NaN();
        } else {
            return quiet_NaN();
        }
    } else if (rhs.is_signaling_NaN()) {
        std::feraiseexcept(FE_INVALID);
        if (rhs.signbit()) {
            return negative_quiet_NaN();
        } else {
            return quiet_NaN();
        }
    } else if (is_quiet_NaN()) {
        return *this;
    } else if (rhs.is_quiet_NaN()) {
        return rhs;
    }

    // handle infinity
    if ((upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
        if (rhs.is_zero()) {
            return negative_quiet_NaN();
        } else if (rhs.signbit()) {
            return -*this;
        } else {
            return *this;
        }
    } else if ((rhs.upper_ & quadruple_exponent_max) == quadruple_exponent_max) {
        if (is_zero()) {
            return negative_quiet_NaN();
        } else if (signbit()) {
            return -rhs;
        } else {
            return rhs;
        }
    }

    if (is_zero() || rhs.is_zero()) {
        return result_sign ? -quadruple{} : quadruple{};
    }

    auto lhs_exp = exponent_to_uint16(upper_);
    auto rhs_exp = exponent_to_uint16(rhs.upper_);
    int adjusted_res_exp = lhs_exp + rhs_exp - exponent_values::quadruple_exponent_bias;
    if (adjusted_res_exp > static_cast<int>(exponent_values::quadruple_exponent_max)) {
        return result_sign ? negative_infinity() : infinity();
    }

    auto res_exp = static_cast<uint16_t>(adjusted_res_exp);

    auto lhs_mantissa = convert_mantissa();
    auto rhs_mantissa = rhs.convert_mantissa();

    auto res_mantissa = lhs_mantissa * rhs_mantissa;
    auto msb = res_mantissa.most_significant_bit_position();
    // 0 adjustment if msb is 64 - upper_bit_size
    int exponent_adj = static_cast<int>(quadruple_exponent_size) - msb;
    if (exponent_adj >= 0) {
        res_exp++;
    }
    res_mantissa.normalize(exponent_adj);
    // remove implied bit
    res_mantissa.upper &= upper_mantissa_mask;

    uint64_t res_exp_shifted = static_cast<uint64_t>(res_exp) << (bit_size_of<uint64_t>() - bit_size_of<uint16_t>());
    auto res = quadruple{res_exp_shifted | res_mantissa.upper, res_mantissa.lower};
    if (result_sign) {
        res.upper_ |= sign_bit_mask;
    }
    return res;
}

quadruple& quadruple::operator+=(const quadruple& rhs) {
    *this = *this + rhs;
    return *this;
}

quadruple& quadruple::operator-=(const quadruple& rhs) {
    *this = *this - rhs;
    return *this;
}

quadruple& quadruple::operator*=(const quadruple& rhs) {
    *this = *this * rhs;
    return *this;
}

bool quadruple::operator==(const quadruple& rhs) const noexcept {
    if (is_NaN() || rhs.is_NaN()) {
        return false;
    }
    if (is_zero() && rhs.is_zero()) {
        return true;
    }

    return upper_ == rhs.upper_ && lower_ == rhs.lower_;
}

bool quadruple::operator!=(const quadruple& rhs) const noexcept { return !operator==(rhs); }

bool quadruple::operator<(const quadruple& rhs) const noexcept {
    if (is_NaN() || rhs.is_NaN()) {
        return false;
    }
    auto lhs_sign = signbit();
    auto rhs_sign = rhs.signbit();
    if (lhs_sign != rhs_sign) {
        return lhs_sign > rhs_sign;
    }

    auto lhs_exp = exponent_to_uint16(upper_);
    auto rhs_exp = exponent_to_uint16(rhs.upper_);

    if (lhs_exp < rhs_exp) {
        return !lhs_sign;
    } else if (lhs_exp > rhs_exp) {
        return lhs_sign;
    } else {
        return (convert_mantissa() < rhs.convert_mantissa()) != lhs_sign;
    }
}

bool quadruple::operator<=(const quadruple& rhs) const noexcept { return operator==(rhs) || operator<(rhs); }

bool quadruple::operator>(const quadruple& rhs) const noexcept { return rhs.operator<(*this); }

bool quadruple::operator>=(const quadruple& rhs) const noexcept { return !operator<(rhs); }

std::partial_ordering quadruple::operator<=>(const quadruple& rhs) const noexcept {
    if (*this == rhs) {
        return std::partial_ordering::equivalent;
    } else if (*this < rhs) {
        return std::partial_ordering::less;
    } else if (*this > rhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::unordered;
    }
}

bool quadruple::mantissa_calc::is_zero() const noexcept { return upper == 0 && lower == 0; }

// returns 128, if there is no bits set
int quadruple::mantissa_calc::most_significant_bit_position() const noexcept {
    if (upper != 0) {
        return ::most_significant_bit_position<uint64_t>(upper);
    } else {
        return ::most_significant_bit_position<uint64_t>(lower) + static_cast<int>(sizeof(lower) * 8);
    }
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
    assert(amount <= sizeof(mantissa_calc) * 8);
    if (amount > sizeof(upper_) * 8) {
        upper = lower << (amount - sizeof(upper_) * 8);
        lower = 0;
    } else {
        uint64_t lower_store = lower;
        lower_store >>= sizeof(lower_store) * 8 - amount;
        lower <<= amount;
        upper <<= amount;
        upper |= lower_store;
    }
}

void quadruple::mantissa_calc::shift_right(uint32_t amount) noexcept {
    if (amount > quadruple_mantissa_size) {
        upper = 0;
        lower = 0;
        return;
    }
    if (amount > sizeof(lower) * 8) {
        lower = upper >> (amount - sizeof(lower) * 8);
        upper = 0;
        return;
    }
    if (amount > upper_bit_size) {
        uint64_t lower_store = lower;
        lower = upper << (sizeof(upper) * 8 - amount);
        lower |= lower_store >> amount;
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

quadruple::mantissa_calc quadruple::mantissa_calc::operator*(const mantissa_calc& rhs) const noexcept {
    // with implicit bit added, we can have up to 113 bits for each mantissa
    // which will become up to 226 bits for the result
    // from which we need only upper half, but lower has to be calculated
    // otherwise result will be incorrect
    // we can split it into 32 bit chunks, multiplying which will give 64 bit ones
    // with 32 bit chunks, result will be 256, for easier use

    std::array<uint32_t, 8> chunks{};

    for (size_t i = 0; i < sizeof(mantissa_calc) / sizeof(uint32_t); i++) {
        for (size_t j = 0; j < sizeof(mantissa_calc) / sizeof(uint32_t); j++) {
            uint32_t* result_pos = chunks.data() + i + j;
            uint32_t lhs_chunk{}, rhs_chunk{};
            std::memcpy(&lhs_chunk, reinterpret_cast<const uint32_t*>(this) + i, sizeof(uint32_t));
            std::memcpy(&rhs_chunk, reinterpret_cast<const uint32_t*>(&rhs) + j, sizeof(uint32_t));

            uint64_t res = static_cast<uint64_t>(lhs_chunk) * static_cast<uint64_t>(rhs_chunk);
            // because of the uint64_t align, we have to cast it to uint32_t
            uint32_t lower_part{}, upper_part{};
            std::memcpy(&lower_part, &res, sizeof(uint32_t));
            std::memcpy(&upper_part, reinterpret_cast<uint32_t*>(&res) + 1, sizeof(uint32_t));
            *result_pos += lower_part;
            if (*result_pos < lower_part) {
                ++(*(result_pos + 1));
            }
            *(result_pos + 1) += upper_part;
            if (*(result_pos + 1) < upper_part) {
                //it is impossible to get overflow to chunk[8], so we are safe
                ++(*(result_pos + 2));
            }
        }
    }
    // after that floating point is 32 bits away from the start, while it is supposed to be 16 (quadruple_exponent_size)
    // chunks 0-2 will not affect the result, and 3rd has only one bit that we care about
    // so shift only 4-7 chunks
    // shifts can be combined into uint64_t

    mantissa_calc result;
    std::memcpy(&result.lower, chunks.data() + 4, sizeof(result.lower));
    std::memcpy(&result.upper, chunks.data() + 6, sizeof(result.upper));
    result.upper <<= quadruple_exponent_size;
    // lower_pair have to move top 15 bits to upper_pair
    uint64_t carry = result.lower >> (bit_size_of<uint64_t>() - quadruple_exponent_size);
    result.lower <<= quadruple_exponent_size;
    result.upper |= carry;

    if (is_bit_set<uint32_t, quadruple_exponent_size>(chunks[3])) {
        // round up
        if (++result.lower == 0) {
            ++result.upper;
        }
    }
    return result;
}

bool quadruple::mantissa_calc::operator<(const mantissa_calc& rhs) const noexcept {
    return upper < rhs.upper || (upper == rhs.upper && lower < rhs.lower);
}

quadruple::mantissa_calc quadruple::convert_mantissa() const {
    static constexpr uint64_t implied_mask = single_bit_mask<uint64_t, quadruple_exponent_size>();
    mantissa_calc result{lower_, upper_ & upper_mantissa_mask};
    if (!is_subnormal()) {
        result.upper |= implied_mask;
    }
    return result;
}
