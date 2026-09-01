# --------------------------------------
# Dependencies
# --------------------------------------

include(FetchContent)

set(USE_SYSTEM_SDL3 OFF)
set(USE_SYSTEM_SDL3_TTF OFF)
set(USE_SYSTEM_SDL3_NET OFF)

option(TABLICA_USE_SDL3_NET_WORKAROUND
        "Statically link SDL3_net on systems without SDL3_net in its repositories"
        ON
)

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

    # Build SDL3_net statically if SDL3 and SDL3_ttf is from the system
    # we are just assuming that the certain OSes does not have SDL3_net in repo
    if(NOT ANDROID AND NOT WIN32 AND USE_SYSTEM_SDL3 AND USE_SYSTEM_SDL3_TTF AND TABLICA_USE_SDL3_NET_WORKAROUND)
        set(_OLD_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
        set(BUILD_SHARED_LIBS OFF)
    endif()

    FetchContent_Declare(
        SDL3_net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
        GIT_TAG main
    )

    if(NOT ANDROID AND NOT WIN32 AND USE_SYSTEM_SDL3 AND USE_SYSTEM_SDL3_TTF AND TABLICA_USE_SDL3_NET_WORKAROUND)
        set(BUILD_SHARED_LIBS "${_OLD_BUILD_SHARED_LIBS}")
        unset(_OLD_BUILD_SHARED_LIBS)
    endif()

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