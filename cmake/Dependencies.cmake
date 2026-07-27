# --------------------------------------
# Dependencies
# --------------------------------------

include(FetchContent)

set(USE_SYSTEM_SDL OFF)

# --------------------------------------
# Try system SDL3 on Linux
# --------------------------------------

if(UNIX AND NOT ANDROID)

    find_package(SDL3 CONFIG QUIET)
    find_package(SDL3_ttf CONFIG QUIET)
    find_package(SDL3_net CONFIG QUIET)

    if(SDL3_FOUND AND SDL3_ttf_FOUND AND SDL3_net_FOUND)
        message(STATUS "Using system SDL3 libraries")
        set(USE_SYSTEM_SDL ON)

    endif()

endif()

# --------------------------------------
# Use FetchContent if unavailable
# --------------------------------------

if(NOT USE_SYSTEM_SDL)

    # SDL3
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3)

    # SDL3_ttf
    FetchContent_Declare(
            SDL3_ttf
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG main
    )

    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3_ttf)


    # SDL3_net
    FetchContent_Declare(
            SDL3_net
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
            GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3_net)

endif()