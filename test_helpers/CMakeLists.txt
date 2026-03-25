project(test_helpers CXX)

set(SOURCE_${PROJECT_NAME}
        test_utils.cpp
)
set(HEADERS_${PROJECT_NAME}
        test_utils.hpp
)

include_directories(${CMAKE_SOURCE_DIR})

add_library(test_helpers ${HEADERS_${PROJECT_NAME}} ${SOURCE_${PROJECT_NAME}})