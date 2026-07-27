cmake_minimum_required(VERSION 3.22)

# --------------------------------------
# Version
# --------------------------------------

include(cmake/Version.cmake)

# --------------------------------------
# Project settings
# --------------------------------------

project(tablica VERSION ${APP_VERSION_STRING} LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# --------------------------------------
# Application information
# --------------------------------------
set(APP_NAME "Tablica Znakowa")
set(APP_COMPANY "ZupaNET Development")

# --------------------------------------
# Dependencies
# --------------------------------------

include(cmake/Dependencies.cmake)

# --------------------------------------
# Application sources and includes
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
# Resources (only for development)
# --------------------------------------

if(NOT ANDROID)

	add_custom_command(
			TARGET tablica POST_BUILD

			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_SOURCE_DIR}/resources/common"
			$<TARGET_FILE_DIR:tablica>/resources
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
		APP_VERSION_STRING="${APP_VERSION_STRING}"
		APP_VERSION_MAJOR=${APP_VERSION_MAJOR}
		APP_VERSION_MINOR=${APP_VERSION_MINOR}
		APP_VERSION_PATCH=${APP_VERSION_PATCH}
)

# --------------------------------------
# Third party libraries
# --------------------------------------

# Inih
add_library(inih STATIC
		third_party/inih/ini.c
)

target_include_directories(inih
		PUBLIC
		third_party/inih
)

# stb_image
add_library(stb_image INTERFACE)

target_include_directories(stb_image
		INTERFACE
		third_party/stb_image
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
		stb_image
)

# --------------------------------------
# Windows resources
# --------------------------------------

if(WIN32)

	target_sources(tablica PRIVATE
		resources/windows/version.rc
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
# InnoSetup installer preparation
# --------------------------------------

if(WIN32)

	configure_file(
			${CMAKE_SOURCE_DIR}/resources/windows/installer.iss.in
			${CMAKE_BINARY_DIR}/installer.iss
			@ONLY
	)

endif()

# --------------------------------------
# Installation on Linux
# --------------------------------------

include(GNUInstallDirs)

install(
	TARGETS tablica
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
	DIRECTORY ${CMAKE_SOURCE_DIR}/resources/common/
	DESTINATION ${CMAKE_INSTALL_DATADIR}/tablica
)

# --------------------------------------
# CPack configuration
# --------------------------------------

set(CPACK_PACKAGE_NAME "${APP_NAME}")
set(CPACK_PACKAGE_VENDOR "${APP_COMPANY}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Tablica Znakowa")
set(CPACK_PACKAGE_CONTACT "devel@zupanet.pl")
set(CPACK_PACKAGE_FILE_NAME "${APP_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "tablica")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# DEB
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${APP_COMPANY}")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://zupanet.pl")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl3-0, libsdl3-ttf0, libsdl3-net0")

# RPM
set(CPACK_RPM_PACKAGE_LICENSE "GPL-2.0-only")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Utilities")
set(CPACK_RPM_PACKAGE_URL "https://example.com")
set(CPACK_RPM_PACKAGE_REQUIRES "SDL3 SDL3_ttf SDL3_net")

set(CPACK_GENERATOR "DEB;RPM;TGZ")

include(CPack)