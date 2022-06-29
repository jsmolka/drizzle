cmake_minimum_required(VERSION 3.8)

project(drizzle)

option(ENABLE_SDL2 "Enable SDL2" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")

if (ENABLE_SDL2)
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})
  add_compile_definitions(DZ_SDL)
endif()

include_directories(src)
include_directories(deps/robin-map/include)
include_directories(deps/sh/include)

file(GLOB_RECURSE SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/src/*.h
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

if (ENABLE_SDL2)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARIES})
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${CMAKE_PROJECT_NAME} stdc++fs)
endif()
