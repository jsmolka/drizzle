cmake_minimum_required(VERSION 3.8)

project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")

include_directories(src)
include_directories(modules)
include_directories(../drizzle/src)
include_directories(../drizzle/modules/shell/include)

file(GLOB_RECURSE SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ../drizzle/src/error.h
  ../drizzle/src/scanner.h
  ../drizzle/src/scanner.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()
