# --------------------------------------
# Dependencies
# --------------------------------------

include(FetchContent)

set(USE_SYSTEM_SDL3 OFF)
set(USE_SYSTEM_SDL3_TTF OFF)
set(USE_SYSTEM_SDL3_NET OFF)

# --------------------------------------
# Try system libraries on Linux
# --------------------------------------

if(UNIX AND NOT ANDROID)

    find_package(SDL3 CONFIG QUIET)
    if(SDL3_FOUND)
        message(STATUS "Using system SDL3")
        set(USE_SYSTEM_SDL3 ON)
    endif()

    find_package(SDL3_ttf CONFIG QUIET)
    if(SDL3_ttf_FOUND)
        message(STATUS "Using system SDL3_ttf")
        set(USE_SYSTEM_SDL3_TTF ON)
    endif()

    find_package(SDL3_net CONFIG QUIET)
    if(SDL3_net_FOUND)
        message(STATUS "Using system SDL3_net")
        set(USE_SYSTEM_SDL3_NET ON)
    endif()

endif()


# --------------------------------------
# Fetch missing libraries
# --------------------------------------

# SDL3
if(NOT USE_SYSTEM_SDL3)

    message(STATUS "Fetching SDL3")

    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3)

endif()


# SDL3_ttf
if(NOT USE_SYSTEM_SDL3_TTF)

    message(STATUS "Fetching SDL3_ttf")

    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3_ttf)

endif()


# SDL3_net
if(NOT USE_SYSTEM_SDL3_NET)

    message(STATUS "Fetching SDL3_net")

    FetchContent_Declare(
        SDL3_net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
        GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3_net)

endif()

# --------------------------------------
# Install runtime dependencies
# --------------------------------------

if(TARGET SDL3-shared AND NOT WIN32)
    install(
        TARGETS SDL3-shared
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if(TARGET SDL3_ttf-shared AND NOT WIN32)
    install(
        TARGETS SDL3_ttf-shared
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if(TARGET SDL3_net-shared AND NOT WIN32)
    install(
        TARGETS SDL3_net-shared
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()