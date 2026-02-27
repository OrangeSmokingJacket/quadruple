project(tests)

set(test_files
        test_arithmetics.cpp
        test_comparison.cpp
        test_conversion.cpp
)

add_executable(${PROJECT_NAME} ${test_files})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        quadruple
        Catch2::Catch2WithMain
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ..
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})