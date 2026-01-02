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

FetchContent_Declare(open-simplex-noise-in-c
    GIT_REPOSITORY https://github.com/nonk123/open-simplex-noise-in-c.git
    GIT_TAG c3862273b9cd7a44d1b2b50c9fc23a9a8cc13992)
set(OSN_FLOAT_TYPE float)

FetchContent_MakeAvailable(raylib open-simplex-noise-in-c)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

set(GENERATED ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED})

function(embed_txt FILENAME)
    file(READ ${CMAKE_SOURCE_DIR}/${FILENAME} CONTENTS)
    file(WRITE ${GENERATED}/${FILENAME} "R\"(${CONTENTS})\"")
endfunction()

embed_txt(shaders/leet.vsh)
embed_txt(shaders/leet.fsh)

embed_txt(shaders/skydome.vsh)
embed_txt(shaders/skydome.fsh)

add_executable(fun ${SOURCES} ${HEADERS})
target_link_libraries(fun PRIVATE raylib open-simplex-noise-in-c)
target_include_directories(fun
    PRIVATE ${CMAKE_SOURCE_DIR}/include ${GENERATED})
target_compile_definitions(fun
    PRIVATE _POSIX_C_SOURCE=200112L _WIN32_WINNT=0x0501 WINVER=0x0501)

add_custom_command(TARGET fun POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:fun>/assets

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:fun>/assets

    COMMAND_EXPAND_LISTS)
