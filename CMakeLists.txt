cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set(CMAKE_CXX_STANDARD 11)

project(build-napi-with-cmake)
include_directories(${CMAKE_JS_INC})
message(STATUS ${CMAKE_JS_INC})
file(GLOB_RECURSE SOURCE_FILES "src/*.c")

find_library(SYSTEMD_LIBRARY systemd)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${SYSTEMD_LIBRARY})

# Define NAPI_VERSION
add_definitions(-DNAPI_VERSION=3)