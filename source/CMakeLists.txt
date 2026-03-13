project(quadruple_source CXX)

set(SOURCE_${PROJECT_NAME}
        quadruple.cpp
)
set(HEADERS_${PROJECT_NAME}
        quadruple.hpp
        utils.hpp
)

if (ENABLE_TESTS OR ENABLE_BENCHMARKS)
    set(test_helpers
            test_helpers/test_utils.hpp
            test_helpers/test_utils.cpp
    )
endif ()

include_directories(${CMAKE_SOURCE_DIR})

add_library(quadruple ${HEADERS_${PROJECT_NAME}} ${SOURCE_${PROJECT_NAME}} ${test_helpers})