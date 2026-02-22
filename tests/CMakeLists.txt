project(tests)

set(SOURCES_${PROJECT_NAME}
        test_conversion.cpp
        test_arithmetics.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES_${PROJECT_NAME}})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        quadruple
        Catch2::Catch2
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ..
)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})