#pragma once

#include <climits>
#include <limits>
#include "utils.hpp"

// There is no requirements for how UB is handled
// This file is needed to handle it consistently with float/double

namespace UB_handle {

    struct unary_signaling {
    private:
        static constexpr bool PRESERVED_FLOAT = is_sNaN(-std::numeric_limits<float>::signaling_NaN());
        static constexpr bool PRESERVED_DOUBLE = is_sNaN(-std::numeric_limits<double>::signaling_NaN());
        static_assert(PRESERVED_FLOAT == PRESERVED_DOUBLE &&
            "unary signaling behaviour is undefined");
    public:
        static constexpr bool PRESERVED =
            PRESERVED_FLOAT == PRESERVED_DOUBLE ? PRESERVED_FLOAT : false;
    };

    // INFINITY_TO_INTEGER
    namespace to_integer_conversion {
        // Since ub can not be checked during compilation, we have to write values manually
#if defined(__GNUC__)
        template <typename T> requires std::is_integral_v<T>
        static constexpr T POS_OVERFLOW = T{0};
        template <typename T> requires std::is_integral_v<T>
        static constexpr T NEG_OVERFLOW = T{0};
        template <typename T> requires std::is_integral_v<T>
        static constexpr T POS_INF = T{0};
        template <typename T> requires std::is_integral_v<T>
        static constexpr T NEG_INF = T{0};
        template <typename T> requires std::is_integral_v<T>
        static constexpr T NaN = T{0};

        template <>
        constexpr int32_t POS_OVERFLOW<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template <>
        constexpr int64_t POS_OVERFLOW<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000

        template <>
        constexpr int32_t NEG_OVERFLOW<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template <>
        constexpr int64_t NEG_OVERFLOW<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000
        template <>
        constexpr uint64_t NEG_OVERFLOW<uint64_t> = 0x8000000000000000;

        template <>
        constexpr int32_t POS_INF<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template <>
        constexpr int64_t POS_INF<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000
        template <>

        constexpr int32_t NEG_INF<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template <>
        constexpr int64_t NEG_INF<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000
        template <>
        constexpr uint64_t NEG_INF<uint64_t> = 0x8000000000000000;

        template <>
        constexpr int32_t NaN<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template <>
        constexpr int64_t NaN<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000
        template <>
        constexpr uint64_t NaN<uint64_t> = 0x8000000000000000;
#endif
    } // namespace to_integer_conversion

} // namespace UB_handle