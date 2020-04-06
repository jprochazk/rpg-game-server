cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(server VERSION "0.1.0" LANGUAGES CXX)

# ---- OPTIONS

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Wpedantic)
elseif(DEFINED MSVC)
    add_compile_options(/W4)
endif()

# ---- DEPENDENCIES ----

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS         OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package(Boost 1.72.0 REQUIRED)

find_package(spdlog CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${spdlog_INCLUDE_DIRS})

# ---- SOURCES ----

set(SERVER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${SERVER_SOURCE_DIR})

add_subdirectory(${SERVER_SOURCE_DIR})
message(STATUS "SERVER_SOURCES = " $ENV{SERVER_SOURCES})

# ---- EXECUTABLE ----

add_executable(server ${SERVER_SOURCE_DIR}/main.cpp $ENV{SERVER_SOURCES})
target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server PRIVATE spdlog::spdlog_header_only)

