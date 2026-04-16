#include <catch2/catch_all.hpp>

#include "quadruple.hpp"
#include "utils.hpp"
#include <cstring>
#include <test_helpers/test_utils.hpp>

TEMPLATE_TEST_CASE("conversion", "[benchmark]", float, double) {
    SECTION("positive") {
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            std::vector<quadruple> conversion_results;
            conversion_results.reserve(generated.size());

            BENCHMARK("to quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    conversion_results.emplace_back(generated[i]);
                }
            };
            BENCHMARK("from quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    generated[i] = static_cast<TestType>(conversion_results[i]);
                }
            };
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            std::vector<quadruple> conversion_results;
            conversion_results.reserve(generated.size());

            BENCHMARK("to quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    conversion_results.emplace_back(generated[i]);
                }
            };
            BENCHMARK("from quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    generated[i] = static_cast<TestType>(conversion_results[i]);
                }
            };
        }
    }

    SECTION("negative") {
        SECTION("random numbers") {
            auto generated = generate_normal_numbers<TestType>(test_size);
            for (auto& value : generated) {
                value *= -1;
            }
            std::vector<quadruple> conversion_results;
            conversion_results.reserve(generated.size());

            BENCHMARK("to quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    conversion_results.emplace_back(generated[i]);
                }
            };
            BENCHMARK("from quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    generated[i] = static_cast<TestType>(conversion_results[i]);
                }
            };
        }
        SECTION("random subnormal numbers") {
            auto generated = generate_subnormal_numbers<TestType>(test_size);
            for (auto& value : generated) {
                value *= -1;
            }
            std::vector<quadruple> conversion_results;
            conversion_results.reserve(generated.size());

            BENCHMARK("to quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    conversion_results.emplace_back(generated[i]);
                }
            };
            BENCHMARK("from quadruple") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    generated[i] = static_cast<TestType>(conversion_results[i]);
                }
            };
        }
    }
}
