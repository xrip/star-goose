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

if (EMSCRIPTEN)
    set(PLATFORM Web CACHE STRING "Platform to build for." FORCE)
    string(REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")
else()
    add_executable(surface_check port/surface_check.c)
    target_link_libraries(surface_check PRIVATE goose_surface)
endif()

include(FetchContent)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_executable(goose port/main.c port/pictures.c port/atlas_data.c port/play.c port/tunnel.c port/maps.c port/hud.c port/sound.c)
target_link_libraries(goose PRIVATE goose_surface raylib)
if (WIN32)
    target_link_libraries(goose PRIVATE winmm gdi32)
endif()
if (EMSCRIPTEN)
    # WindowShouldClose sleeps through emscripten_sleep, so the desktop loop
    # stays. Pages cannot send COOP/COEP, so this build does not use pthreads.
    set_target_properties(goose PROPERTIES SUFFIX ".html")
    target_link_options(goose PRIVATE
        "SHELL:-s WASM=1"
        "SHELL:-s ASYNCIFY=1"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s INITIAL_MEMORY=67108864"
        "SHELL:-s MINIFY_HTML=0"
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/web/shell.html"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@assets")
endif()
