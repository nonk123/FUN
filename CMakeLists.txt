cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)

project(fun)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5)
FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(fun ${SOURCES} ${HEADERS})
target_link_libraries(fun PRIVATE raylib)
target_include_directories(fun
    PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(fun
    PRIVATE _POSIX_C_SOURCE=200112L _WIN32_WINNT=0x0501 WINVER=0x0501)
