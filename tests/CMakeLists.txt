cmake_minimum_required(VERSION 3.8)

project(tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")

include_directories(src)
include_directories(deps/ut/include)
include_directories(../drizzle/src)
include_directories(../drizzle/deps/robin-map/include)
include_directories(../drizzle/deps/sh/include)

file(GLOB_RECURSE SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ../drizzle/src/*.h
  ../drizzle/src/*.cpp
)

list(FILTER SOURCE_FILES EXCLUDE REGEX ".*/drizzle/src/main.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()
