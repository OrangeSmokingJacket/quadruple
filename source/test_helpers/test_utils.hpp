#pragma once
#include <cmath>
#include <cstdint>
#include <vector>

class quadruple;

static constexpr size_t benchmark_size = 10000;
// TODO: increase to million or bigger
static constexpr size_t test_size = 10000;

// generates sequence via mt19937(_64) using default seed for the repeatability (only positive values)
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

template<typename T>
void remove_NaNs(std::vector<T>& vector) {
    for (auto it = vector.begin(); it != vector.end();) {
        if (std::isnan(*it)) {
            vector.erase(it);
        } else {
            ++it;
        }
    }
}
template<>
void remove_NaNs(std::vector<quadruple>& vector);