project(benchmarks)

set(benchmark_files
        benchmark_arithmetics.cpp
        benchmark_conversion.cpp
)

add_executable(${PROJECT_NAME} ${benchmark_files})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        quadruple
        test_helpers
        Catch2::Catch2WithMain
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ..
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})