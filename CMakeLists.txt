cmake_minimum_required(VERSION 3.28.0 FATAL_ERROR)

project(fun)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5)

FetchContent_Declare(noise
    GIT_REPOSITORY https://github.com/smcameron/open-simplex-noise-in-c.git
    GIT_TAG 53cf5bfd4a5289f263568bedd778e2d86eb8a1ef)

FetchContent_MakeAvailable(raylib noise)

add_library(noise
    ${noise_SOURCE_DIR}/open-simplex-noise.c
    ${noise_SOURCE_DIR}/open-simplex-noise.h)
target_include_directories(noise PUBLIC ${noise_SOURCE_DIR})
target_compile_definitions(noise PUBLIC OSNFLOAT=float)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

set(GENERATED ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED})

function(embed_txt FILENAME)
    file(READ ${CMAKE_SOURCE_DIR}/${FILENAME} CONTENTS)
    file(WRITE ${GENERATED}/${FILENAME} "R\"(${CONTENTS})\"")
endfunction()

embed_txt(shaders/base.vsh)
embed_txt(shaders/base.fsh)
embed_txt(shaders/terrain.fsh)

add_executable(fun ${SOURCES} ${HEADERS})
target_link_libraries(fun PRIVATE raylib noise)
target_include_directories(fun
    PRIVATE ${CMAKE_SOURCE_DIR}/include ${GENERATED})
target_compile_definitions(fun
    PRIVATE _POSIX_C_SOURCE=200112L _WIN32_WINNT=0x0501 WINVER=0x0501)
