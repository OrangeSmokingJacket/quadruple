#pragma once
#include <cmath>
#include <cstdint>
#include <vector>

class quadruple;

static constexpr size_t benchmark_size = 10000;
// TODO: increase to million or bigger
static constexpr size_t test_size = 10000;

// generates sequence via mt19937(_64) using default seed for the repeatability
// (only positive values)
template <typename T>
std::vector<T> generate_normal_numbers(size_t count);
template <typename T>
std::vector<T> generate_subnormal_numbers(size_t count);

template <>
std::vector<int8_t> generate_normal_numbers(size_t count);
template <>
std::vector<int16_t> generate_normal_numbers(size_t count);
template <>
std::vector<int32_t> generate_normal_numbers(size_t count);
template <>
std::vector<int64_t> generate_normal_numbers(size_t count);
template <>
std::vector<uint8_t> generate_normal_numbers(size_t count);
template <>
std::vector<uint16_t> generate_normal_numbers(size_t count);
template <>
std::vector<uint32_t> generate_normal_numbers(size_t count);
template <>
std::vector<uint64_t> generate_normal_numbers(size_t count);
template <>
std::vector<float> generate_normal_numbers(size_t count);
template <>
std::vector<double> generate_normal_numbers(size_t count);
template <>
std::vector<quadruple> generate_normal_numbers(size_t count);
template <>
std::vector<float> generate_subnormal_numbers(size_t count);
template <>
std::vector<double> generate_subnormal_numbers(size_t count);
template <>
std::vector<quadruple> generate_subnormal_numbers(size_t count);

#if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)

template <>
std::vector<__int128> generate_normal_numbers(size_t count);
template <>
std::vector<unsigned __int128> generate_normal_numbers(size_t count);

#endif

template <typename T>
void remove_NaNs(std::vector<T>& vector) {
    for (auto it = vector.begin(); it != vector.end();) {
        if (std::isnan(*it)) {
            vector.erase(it);
        } else {
            ++it;
        }
    }
}
template <>
void remove_NaNs(std::vector<quadruple>& vector);

template <typename Iterator1, typename Iterator2, typename Function>
void for_each_premutation(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2, Function&& func) {
    for (; begin1 != end1; ++begin1) {
        for (; begin2 != end2; ++begin2) {
            func(*begin1, *begin2);
        }
    }
}

template <typename Container1, typename Container2, typename Function>
void for_each_premutation(const Container1& container1, const Container2& container2, Function&& func) {
    for_each_premutation(container1.begin(),
                         container1.end(),
                         container2.begin(),
                         container2.end(),
                         std::forward<Function>(func));
}

namespace impl {

    template <typename LeftType, typename Packer>
    struct one_side_permutation;

    template <typename LeftType, template <typename...> typename Packer, typename... RightTypes>
    struct one_side_permutation<LeftType, Packer<RightTypes...>> {
        using type = Packer<Packer<LeftType, RightTypes>...>;
    };

} // namespace impl

template <typename Packer, typename... Types>
struct merge_types;

template <template <typename...> typename Packer, typename... Types>
struct merge_types<Packer<Types...>> {
    using type = Packer<Types...>;
};

template <template <typename...> typename Packer, typename... Ts, typename... Us, typename... Rest>
struct merge_types<Packer<Ts...>, Packer<Us...>, Rest...> {
    using type = merge_types<Packer<Ts..., Us...>, Rest...>::type;
};

template <typename Packer, typename T>
struct types_cross_product;

template <template <typename...> typename Packer, typename... LeftTypes, typename... RightTypes>
struct types_cross_product<Packer<LeftTypes...>, Packer<RightTypes...>> {
    using type = merge_types<typename impl::one_side_permutation<LeftTypes, Packer<RightTypes...>>::type...>::type;
};

template <typename T>
struct rebind;

template <template <typename...> class C, typename... Args>
struct rebind<C<Args...>> {
    template <typename... NewArgs>
    using with = C<NewArgs...>;
};

template <typename T, typename... NewArgs>
using rebind_with = typename rebind<T>::template with<NewArgs...>;

#if defined(EXTENSIONS) && defined(__SIZEOF_INT128__)
using integer_types =
    std::tuple<int8_t, int16_t, int32_t, int64_t, __int128, uint8_t, uint16_t, uint32_t, uint64_t, unsigned __int128>;
#else
using integer_types = std::tuple<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;
#endif

using all_constructable_types = merge_types<integer_types, std::tuple<float, double>>::type;
