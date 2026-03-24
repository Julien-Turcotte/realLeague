cmake_minimum_required(VERSION 3.20)
project(realLeague VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.10/SDL3-3.2.10.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SDL3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)

file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp"
)
file(GLOB_RECURSE GAME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp"
)
set(SRC_SOURCES
    src/main.cpp
    src/Game.cpp
)

add_executable(realLeague
    ${SRC_SOURCES}
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
)

target_include_directories(realLeague PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SDL2TTF_INCLUDE_DIRS}
)

target_link_libraries(realLeague PRIVATE SDL3::SDL3 ${SDL2TTF_LIBRARIES})

if(MSVC)
    target_compile_options(realLeague PRIVATE /W4)
else()
    target_compile_options(realLeague PRIVATE -Wall -Wextra -Wpedantic)
endif()
