#pragma once
#include <vector>
#include <random>

// TODO: increase to million or bigger
static constexpr size_t test_size = 10000;

// generates sequence via mt19937(_64) using default seed for the repeatability (only positive values)
template <typename T> requires std::is_floating_point_v<T>
std::vector<T> generate_normal_numbers(size_t count);
template <typename T> requires std::is_floating_point_v<T>
std::vector<T> generate_subnormal_numbers(size_t count);

template <>
std::vector<float> generate_normal_numbers(size_t count);
template <>
std::vector<double> generate_normal_numbers(size_t count);
template <>
std::vector<float> generate_subnormal_numbers(size_t count);
template <>
std::vector<double> generate_subnormal_numbers(size_t count);

template<typename T> requires std::is_floating_point_v<T>
void remove_NaNs(std::vector<T>& vector) {
    for (auto it = vector.begin(); it != vector.end();) {
        if (std::isnan(*it)) {
            vector.erase(it);
        } else {
            ++it;
        }
    }
}