cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)

project(server VERSION "0.1.0" LANGUAGES CXX)

# ---- OPTIONS ----

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# can't get it to work otherwise
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Wpedantic)
elseif(DEFINED MSVC)
    add_compile_options(/W4)
endif()

# ---- EXECUTABLE ----

add_executable(server src/main.cpp)

# ---- DEPENDENCIES ----

# BOOST
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS         OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package(Boost 1.72.0 REQUIRED)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${Boost_LIBRARIES})

# SPDLOG
find_package(spdlog CONFIG REQUIRED)
target_include_directories(server PRIVATE ${spdlog_INCLUDE_DIRS})
target_link_libraries(server PRIVATE spdlog::spdlog_header_only)

# ---- SOURCES ----
include_directories(${CMAKE_SOURCE_DIR}/src)
set(
    MY_SOURCES
    # common
    src/common/endianness.h
    src/common/ByteBuffer.cpp
    # network
    src/network/SocketListener.cpp
    src/network/Websocket.cpp
    # world
    src/world/World.cpp
    src/world/WorldSocketManager.cpp
    src/world/WorldSession.cpp
    src/world/WorldTime.cpp
)
target_sources(server PRIVATE ${MY_SOURCES})
