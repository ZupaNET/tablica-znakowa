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
# Application information
# --------------------------------------
set(APP_NAME "Tablica Znakowa")
set(APP_COMPANY "ZupaNET Development")

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

if(ANDROID)

	add_library(tablica SHARED
			${APP_SOURCES}
	)

else()

	add_executable(tablica
			${APP_SOURCES}
	)

endif()

target_include_directories(tablica
	PRIVATE src
)

# --------------------------------------
# Resources and Libraries
# --------------------------------------

set(APP_RESOURCES_DIR
		${CMAKE_SOURCE_DIR}/resources/common
)

if(NOT ANDROID)

	add_custom_command(
			TARGET tablica POST_BUILD

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			${APP_RESOURCES_DIR}
			$<TARGET_FILE_DIR:tablica>/resources

			COMMENT "Copying resources"
	)

endif()

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
# Application info definitions
# --------------------------------------

target_compile_definitions(tablica PRIVATE
		APP_NAME="${APP_NAME}"
		APP_COMPANY="${APP_COMPANY}"
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
# GNU GCC static runtime for Windows
# --------------------------------------

if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    target_link_options(tablica PRIVATE
        -static-libgcc
        -static-libstdc++
    )

endif()

# --------------------------------------
# Target for APK generation
# --------------------------------------

if(ANDROID)

	include(cmake/AndroidRuntime.cmake)

	set(JNI_DIR "${CMAKE_BINARY_DIR}/android/app/src/main/jniLibs/${ANDROID_ABI}")

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(GRADLE_TASK assembleDebug)
	else()
		set(GRADLE_TASK assembleRelease)
	endif()

	add_custom_target(android_prepare
			DEPENDS tablica

			COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/android"

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_SOURCE_DIR}/android"
			"${CMAKE_BINARY_DIR}/android"

			COMMAND ${CMAKE_COMMAND} -E make_directory
			"${CMAKE_BINARY_DIR}/android/app/src/main/assets"

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${APP_RESOURCES_DIR}"
			"${CMAKE_BINARY_DIR}/android/app/src/main/assets"

			COMMAND ${CMAKE_COMMAND} -E make_directory "${JNI_DIR}"

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_BINARY_DIR}/lib"
			"${JNI_DIR}"

			COMMAND ${CMAKE_COMMAND} -E copy
			"${ANDROID_LIBCXX_SHARED}"
			"${JNI_DIR}/"

			COMMAND ${CMAKE_COMMAND} -E copy
			"${CMAKE_BINARY_DIR}/build.gradle"
			"${CMAKE_BINARY_DIR}/android/app/build.gradle"

			COMMENT "Copying libraries and resources into APK"
	)

	add_custom_target(apk
			COMMAND gradlew.bat ${GRADLE_TASK} --console=plain
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/android
			DEPENDS tablica android_prepare
			USES_TERMINAL

			COMMENT "Building the APK"
	)

endif()

# --------------------------------------
# Target for Windows install generation
# --------------------------------------

if(WIN32)

	include(cmake/FindInnoSetup.cmake)

	add_custom_target(installer
			COMMAND ${INNO_SETUP_COMPILER} "${CMAKE_BINARY_DIR}/installer.iss"
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			DEPENDS tablica
			USES_TERMINAL

			COMMENT "Building the installer"
	)

	add_custom_target(zip-package
			COMMAND
			${CMAKE_COMMAND} -E tar
			"cfv"
			"${CMAKE_BINARY_DIR}/Output/tablica-znakowa-${APP_VERSION_STRING}-package.zip"
			--format=zip
			.

			WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

			COMMENT "Creating ZIP package"
	)

endif()