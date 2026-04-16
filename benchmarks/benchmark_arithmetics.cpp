#include <catch2/catch_all.hpp>

#include "quadruple.hpp"
#include "utils.hpp"
#include <cstring>
#include <test_helpers/test_utils.hpp>

TEMPLATE_TEST_CASE("arithmetics", "[benchmark]", float, double) {
    SECTION("positive") {
        SECTION("random numbers") {
            auto generated1 = generate_normal_numbers<TestType>(test_size);
            std::vector<TestType> generated2;
            generated2.resize(benchmark_size);
            std::reverse_copy(generated1.begin(), generated1.end(), generated2.begin());
            std::vector<quadruple> converted1;
            std::vector<quadruple> converted2;
            std::vector<quadruple> results;
            converted1.reserve(benchmark_size);
            converted2.reserve(benchmark_size);
            results.reserve(benchmark_size);

            for (size_t i = 0; i < benchmark_size; i++) {
                converted1.emplace_back(generated1[i]);
                converted2.emplace_back(generated2[i]);
            }

            BENCHMARK("addition") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] + converted2[i]);
                }
            };
            BENCHMARK("subtraction") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] - converted2[i]);
                }
            };
        }
        SECTION("random subnormal numbers") {
            auto generated1 = generate_subnormal_numbers<TestType>(test_size);
            std::vector<TestType> generated2;
            generated2.resize(benchmark_size);
            std::reverse_copy(generated1.begin(), generated1.end(), generated2.begin());
            std::vector<quadruple> converted1;
            std::vector<quadruple> converted2;
            std::vector<quadruple> results;
            converted1.reserve(benchmark_size);
            converted2.reserve(benchmark_size);
            results.reserve(benchmark_size);

            for (size_t i = 0; i < benchmark_size; i++) {
                converted1.emplace_back(generated1[i]);
                converted2.emplace_back(generated2[i]);
            }

            BENCHMARK("addition") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] + converted2[i]);
                }
            };
            BENCHMARK("subtraction") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] - converted2[i]);
                }
            };
        }
    }

    SECTION("negative") {
        SECTION("random numbers") {
            auto generated1 = generate_normal_numbers<TestType>(test_size);
            std::vector<TestType> generated2;
            generated2.resize(benchmark_size);
            std::reverse_copy(generated1.begin(), generated1.end(), generated2.begin());
            std::vector<quadruple> converted1;
            std::vector<quadruple> converted2;
            std::vector<quadruple> results;
            converted1.reserve(benchmark_size);
            converted2.reserve(benchmark_size);
            results.reserve(benchmark_size);

            for (size_t i = 0; i < benchmark_size; i++) {
                converted1.emplace_back(generated1[i]);
                converted2.emplace_back(generated2[i]);
            }

            BENCHMARK("addition") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] + converted2[i]);
                }
            };
            BENCHMARK("subtraction") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] - converted2[i]);
                }
            };
        }
        SECTION("random subnormal numbers") {
            auto generated1 = generate_subnormal_numbers<TestType>(test_size);
            std::vector<TestType> generated2;
            generated2.resize(benchmark_size);
            std::reverse_copy(generated1.begin(), generated1.end(), generated2.begin());
            std::vector<quadruple> converted1;
            std::vector<quadruple> converted2;
            std::vector<quadruple> results;
            converted1.reserve(benchmark_size);
            converted2.reserve(benchmark_size);
            results.reserve(benchmark_size);

            for (size_t i = 0; i < benchmark_size; i++) {
                converted1.emplace_back(generated1[i]);
                converted2.emplace_back(generated2[i]);
            }

            BENCHMARK("addition") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] + converted2[i]);
                }
            };
            BENCHMARK("subtraction") {
                for (size_t i = 0; i < benchmark_size; i++) {
                    results.emplace_back(converted1[i] - converted2[i]);
                }
            };
        }
    }
}
