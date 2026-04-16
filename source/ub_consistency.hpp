#pragma once

#include "utils.hpp"
#include <climits>
#include <limits>

// There is no requirements for how UB is handled
// This file is needed to handle it consistently with float/double

namespace UB_handle {

    struct unary_signaling {
    private:
        static constexpr bool PRESERVED_FLOAT = is_sNaN(-std::numeric_limits<float>::signaling_NaN());
        static constexpr bool PRESERVED_DOUBLE = is_sNaN(-std::numeric_limits<double>::signaling_NaN());
        static_assert(PRESERVED_FLOAT == PRESERVED_DOUBLE && "unary signaling behaviour is undefined");

    public:
        static constexpr bool PRESERVED = PRESERVED_FLOAT == PRESERVED_DOUBLE ? PRESERVED_FLOAT : false;
    };

    namespace to_integer_conversion {
// Since ub can not be checked during compilation, we have to write values
// manually
#if defined(__GNUC__)
        template<typename T>
        requires std::is_integral_v<T>
        static constexpr T NaN = T{0};

    #if defined(NDEBUG) && defined(__OPTIMIZE__)
        template<typename T>
        requires std::is_integral_v<T>
        static constexpr T POS_OVERFLOW = std::numeric_limits<T>::max();
        template<typename T>
        requires std::is_integral_v<T>
        static constexpr T NEG_OVERFLOW = std::numeric_limits<T>::min();

        #if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)
        template<>
        constexpr __int128 POS_OVERFLOW<__int128> = static_cast<__int128>(
            ((unsigned __int128){0x7FFFFFFFFFFFFFFF} << 64) + (unsigned __int128){0xFFFFFFFFFFFFFFFF});
        template<>
        constexpr unsigned __int128 POS_OVERFLOW<unsigned __int128> = (unsigned __int128){0} - (unsigned __int128){1};

        template<>
        constexpr __int128 NEG_OVERFLOW<__int128> = (__int128{0x8000000000000000} << 64);
        #endif

    #else
        template<typename T>
        requires std::is_integral_v<T>
        static constexpr T NEG_OVERFLOW = T{0};

        template<>
        constexpr int32_t NaN<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template<>
        constexpr int64_t NaN<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000
        template<>
        constexpr uint64_t NaN<uint64_t> = 0x8000000000000000;

        template<typename T>
        requires std::is_integral_v<T>
        static constexpr T POS_OVERFLOW = T{0};

        template<>
        constexpr int32_t POS_OVERFLOW<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template<>
        constexpr int64_t POS_OVERFLOW<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000

        template<>
        constexpr int32_t NEG_OVERFLOW<int32_t> = std::numeric_limits<int32_t>::min(); // 0x80000000
        template<>
        constexpr int64_t NEG_OVERFLOW<int64_t> = std::numeric_limits<int64_t>::min(); // 0x8000000000000000
        template<>
        constexpr uint64_t NEG_OVERFLOW<uint64_t> = 0x8000000000000000;

        #if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)
        template<>
        constexpr __int128 NaN<__int128> = (__int128{0x8000000000000000} << 64) + __int128{0x8000000000000000};
        template<>
        constexpr unsigned __int128 NEG_OVERFLOW<unsigned __int128> =
            ((unsigned __int128){0x8000000000000000} << 64) + (unsigned __int128){0x8000000000000000};
        template<>
        constexpr unsigned __int128 NaN<unsigned __int128> =
            ((unsigned __int128){0x8000000000000000} << 64) + (unsigned __int128){0x8000000000000000};
        #endif
    #endif
#endif
    } // namespace to_integer_conversion

} // namespace UB_handle
