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
pkg_check_modules(SDL3TTF QUIET SDL3_ttf)
pkg_check_modules(SDL2TTF QUIET SDL2_ttf)

set(SDL_TTF_FOUND OFF)
if(SDL3TTF_FOUND)
    set(SDL_TTF_INCLUDE_DIRS ${SDL3TTF_INCLUDE_DIRS})
    set(SDL_TTF_LIBRARIES    ${SDL3TTF_LIBRARIES})
    set(SDL_TTF_FOUND ON)
elseif(SDL2TTF_FOUND)
    set(SDL_TTF_INCLUDE_DIRS ${SDL2TTF_INCLUDE_DIRS})
    set(SDL_TTF_LIBRARIES    ${SDL2TTF_LIBRARIES})
    set(SDL_TTF_FOUND ON)
endif()

target_include_directories(realLeague PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(SDL_TTF_FOUND)
    target_include_directories(realLeague PRIVATE ${SDL_TTF_INCLUDE_DIRS})
    target_link_libraries(realLeague PRIVATE ${SDL_TTF_LIBRARIES})
endif()

if(MSVC)
    target_compile_options(realLeague PRIVATE /W4)
else()
    target_compile_options(realLeague PRIVATE -Wall -Wextra -Wpedantic)
endif()
