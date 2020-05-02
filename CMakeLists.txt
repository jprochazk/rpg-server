cmake_minimum_required(VERSION 3.17.0 FATAL_ERROR)
project(server VERSION "0.1.0" LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Wpedantic)
elseif(DEFINED MSVC)
    add_compile_options(/W4)
    add_compile_options(/bigobj)
endif()

include(conanbuildinfo_multi.cmake)
conan_basic_setup()

add_executable(server src/main.cpp)
conan_target_link_libraries(server)

target_precompile_headers(server PRIVATE src/pch.h)
include_directories(${CMAKE_SOURCE_DIR}/src)
set(MY_SOURCES
    src/pch.cpp
    # common
    src/common/ByteBuffer.cpp
    # network
    src/network/SocketListener.cpp
    src/network/Websocket.cpp
    # world
    src/world/Opcode.cpp
    src/world/World.cpp
    src/world/WorldPacket.cpp
    src/world/WorldSocketManager.cpp
    src/world/WorldSession.cpp
    src/world/WorldTime.cpp
)
target_sources(server PRIVATE ${MY_SOURCES})