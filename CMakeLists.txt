cmake_minimum_required(VERSION 3.16)
project(goose C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(goose_surface
    port/surface.c
    port/terrain.c
    port/terrain_tables.c
    port/level0_tiles.c
    port/goose_model.c)
target_include_directories(goose_surface PUBLIC port)

add_executable(surface_check port/surface_check.c)
target_link_libraries(surface_check PRIVATE goose_surface)

include(FetchContent)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_executable(goose port/main.c port/pictures.c port/atlas_data.c port/play.c port/tunnel.c)
target_link_libraries(goose PRIVATE goose_surface raylib)
if (WIN32)
    target_link_libraries(goose PRIVATE winmm gdi32)
endif()
