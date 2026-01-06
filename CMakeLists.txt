cmake_minimum_required(VERSION 3.10.0)
project(openspm-repo VERSION 0.1.0 LANGUAGES C CXX)
include(FetchContent)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBARCHIVE REQUIRED libarchive)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
)

FetchContent_MakeAvailable(yaml-cpp)

file(GLOB SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(openspm_lib ${SOURCES})
target_link_libraries(openspm_lib PUBLIC yaml-cpp ${LIBARCHIVE_LIBRARIES})
target_include_directories(openspm_lib PUBLIC ${LIBARCHIVE_INCLUDE_DIRS})
add_executable(openspm-repo main.cpp)
target_link_libraries(openspm-repo PRIVATE openspm_lib)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
