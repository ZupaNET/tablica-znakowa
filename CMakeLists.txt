cmake_minimum_required(VERSION 3.28)

project(tablica
	VERSION 1.0.3
	LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
	"${CMAKE_BINARY_DIR}/bin"
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
	"${CMAKE_BINARY_DIR}/lib"
)

# --------------------------------------
# Version
# --------------------------------------

include(cmake/Version.cmake)


# --------------------------------------
# Dependencies
# --------------------------------------

include(FetchContent)

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

# --------------------------------------
# Application
# --------------------------------------

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

add_executable(tablica
	${APP_SOURCES}
)

target_include_directories(tablica
	PRIVATE src
)

# --------------------------------------
# Resources
# --------------------------------------

set(APP_RESOURCES_DIR
		${CMAKE_SOURCE_DIR}/resources/common
)

add_custom_command(
		TARGET tablica POST_BUILD

		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${APP_RESOURCES_DIR}
		$<TARGET_FILE_DIR:tablica>/resources

		COMMENT "Copying resources"
)

# --------------------------------------
# Platform definitions
# --------------------------------------

if(ANDROID)

	target_compile_definitions(tablica PRIVATE
			PLATFORM_ANDROID
	)

elseif(WIN32)

	target_compile_definitions(tablica PRIVATE
			PLATFORM_WINDOWS
	)

elseif(UNIX)

	target_compile_definitions(tablica PRIVATE
			PLATFORM_LINUX
	)

endif()


# --------------------------------------
# Application info
# --------------------------------------

target_compile_definitions(tablica PRIVATE
		APP_NAME="Tablica Znakowa"
		APP_COMPANY="ZupaNET Development"
		APP_VERSION="${PROJECT_VERSION}"
)

# --------------------------------------
# Third party
# --------------------------------------

add_library(inih STATIC
		third_party/inih/ini.c
)

target_include_directories(inih
		PUBLIC
		third_party/inih
)

# --------------------------------------
# Libraries
# --------------------------------------

target_link_libraries(tablica
	PRIVATE
		SDL3::SDL3
		SDL3_ttf::SDL3_ttf
		SDL3_net::SDL3_net
		inih
)

# --------------------------------------
# Windows resources
# --------------------------------------

if(WIN32)

	target_sources(tablica PRIVATE
		${CMAKE_BINARY_DIR}/version.rc
		resources/windows/app.rc
	)
endif()

# --------------------------------------
# MinGW static runtime
# --------------------------------------

if(WIN32 AND MINGW)

    target_link_options(tablica PRIVATE
        -static-libgcc
        -static-libstdc++
    )

endif()