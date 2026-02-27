project(quadruple_source CXX)

set(SOURCE_${PROJECT_NAME}
        quadruple.cpp
)
set(HEADERS_${PROJECT_NAME}
        quadruple.hpp
        utils.hpp
)

include_directories(${CMAKE_SOURCE_DIR})

add_library(quadruple ${HEADERS_${PROJECT_NAME}} ${SOURCE_${PROJECT_NAME}})