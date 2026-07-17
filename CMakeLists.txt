cmake_minimum_required(VERSION 4.3)
project(vMKEiA C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

# --------------------------------------
# Windows static runtime (MinGW)
# --------------------------------------
if(WIN32 AND MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
endif()

# --------------------------------------
# SDL3
# --------------------------------------
if(WIN32 OR APPLE)
    message(STATUS "Using vendored SDL3")

    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    message(STATUS "Using system SDL3")
    find_package(SDL3 REQUIRED CONFIG)
endif()

# --------------------------------------
# SDL3_ttf
# --------------------------------------
if(WIN32 OR APPLE)
    message(STATUS "Using vendored SDL3_ttf")
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)

    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)
else()
    message(STATUS "Using system SDL3_ttf")
    find_package(SDL3_ttf REQUIRED CONFIG)
endif()

# --------------------------------------
# SDL3_net
# --------------------------------------
if(WIN32 OR APPLE)
    message(STATUS "Using vendored SDL3_net")

    add_subdirectory(vendored/SDL_net EXCLUDE_FROM_ALL)
else()
    message(STATUS "Using system SDL3_net")
    find_package(SDL3_net REQUIRED CONFIG)
endif()

if(WIN32)
    add_executable(vMKEiA
            main.c
            app.rc
            fonts.c
    )
else()
    add_executable(vMKEiA
            main.c
            fonts.c
    )
endif()

target_link_libraries(vMKEiA PRIVATE SDL3_net::SDL3_net SDL3_ttf::SDL3_ttf SDL3::SDL3)

# --------------------------------------
# Static MinGW runtime
# --------------------------------------
if(WIN32 AND MINGW)
    target_link_options(vMKEiA PRIVATE
            -static-libgcc
            -static-libstdc++
    )
endif()