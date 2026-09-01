cmake_minimum_required(VERSION 3.30)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# --------------------------------------
# Position Independent Code settings
# --------------------------------------

include(CheckPIESupported)
check_pie_supported()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------
# Application information
# --------------------------------------
set(APP_NAME "Tablica Znakowa")
set(APP_COMPANY "ŻupaNET Development")
if(HAIKU)
string(REGEX REPLACE "[ \t]" "" APP_PACKAGE_NAME "${APP_NAME}")
else()
string(TOLOWER "${APP_NAME}" APP_PACKAGE_NAME)
string(REGEX REPLACE "[^a-z0-9]+" "-" APP_PACKAGE_NAME "${APP_PACKAGE_NAME}")
string(REGEX REPLACE "^-|-$" "" APP_PACKAGE_NAME "${APP_PACKAGE_NAME}")
endif()

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

list(FILTER APP_SOURCES EXCLUDE REGEX "/platform/")

if(WIN32)
	file(GLOB PLATFORM_SOURCES CONFIGURE_DEPENDS
		src/platform/windows/*.c
	)
elseif(ANDROID)
	file(GLOB PLATFORM_SOURCES CONFIGURE_DEPENDS
		src/platform/android/*.c
	)
elseif(LINUX)
	file(GLOB PLATFORM_SOURCES CONFIGURE_DEPENDS
		src/platform/linux/*.c
	)
elseif(HAIKU)
	file(GLOB PLATFORM_SOURCES CONFIGURE_DEPENDS
		src/platform/haiku/*.c
	)
endif()

list(APPEND APP_SOURCES ${PLATFORM_SOURCES})

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
# Application info definitions
# --------------------------------------

target_compile_definitions(tablica PRIVATE
		APP_NAME="${APP_NAME}"
		APP_PACKAGE_NAME="${APP_PACKAGE_NAME}"
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

if(UNIX AND NOT APPLE)
	target_link_libraries(tablica PRIVATE m)
endif()

# --------------------------------------
# Haiku resources
# --------------------------------------

include(cmake/HaikuResources.cmake)

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
# Installation
# --------------------------------------

include(GNUInstallDirs)

if(WIN32)

	install(
		TARGETS tablica
		RUNTIME DESTINATION .
	)

	install(
		DIRECTORY "${CMAKE_SOURCE_DIR}/resources/common/"
		DESTINATION resources
	)

	install(
		FILES
		$<TARGET_FILE:SDL3::SDL3>
		$<TARGET_FILE:SDL3_ttf::SDL3_ttf>
		$<TARGET_FILE:SDL3_net::SDL3_net>
		DESTINATION .
	)

elseif(HAIKU)

	install(
			TARGETS tablica
			RUNTIME DESTINATION "apps/${APP_PACKAGE_NAME}"
	)

	install(
			DIRECTORY "${CMAKE_SOURCE_DIR}/resources/common/"
			DESTINATION "apps/${APP_PACKAGE_NAME}/resources"
	)

else()

	install(
		TARGETS tablica
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)

	install(
		DIRECTORY "${CMAKE_SOURCE_DIR}/resources/common/"
		DESTINATION "${CMAKE_INSTALL_DATADIR}/${APP_PACKAGE_NAME}"
	)

endif()

# --------------------------------------
# CPack configuration
# --------------------------------------

if(NOT WIN32)
	set(CPACK_PACKAGE_NAME "${APP_PACKAGE_NAME}")
else()
	set(CPACK_PACKAGE_NAME "${APP_NAME}")
endif()
set(CPACK_PACKAGE_VENDOR "${APP_COMPANY}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
	"Tablica Znakowa to emulator klasycznej znakowej tablicy LED od firmy MKEiA.")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://zupanet.pl/tablica-znakowa")
set(CPACK_PACKAGE_CONTACT "dev@zupanet.pl")
set(CPACK_PACKAGE_FILE_NAME "${APP_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
if(NOT WIN32)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "tablica")
else()
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "${APP_NAME}")
endif()
set(CPACK_PACKAGE_CHECKSUM SHA256)
if(WIN32)
	set(CPACK_COMPONENTS_ALL Unspecified)
endif()

set(CPACK_PACKAGE_EXECUTABLES "tablica;Tablica Znakowa")
set(CPACK_CREATE_DESKTOP_LINKS tablica)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# DEB
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${APP_COMPANY} <${CPACK_PACKAGE_CONTACT}>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
if(USE_SYSTEM_SDL)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl3-0, libsdl3-ttf0, libsdl3-net0")
endif()

# RPM
set(CPACK_RPM_PACKAGE_LICENSE "GPL-2.0-only")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Utilities")
set(CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE}")
if(USE_SYSTEM_SDL)
	set(CPACK_RPM_PACKAGE_REQUIRES "SDL3 SDL3_ttf SDL3_net")
endif()

# InnoSetup
set(CPACK_INNOSETUP_ALLOW_CUSTOM_DIRECTORY ON)
set(CPACK_INNOSETUP_LANGUAGES "polish")
set(CPACK_INNOSETUP_IGNORE_README_PAGE ON)
set(CPACK_INNOSETUP_USE_MODERN_WIZARD ON)
set(CPACK_INNOSETUP_PROGRAM_MENU_FOLDER "${APP_NAME}")
set(CPACK_INNOSETUP_headers_INSTALL_DIRECTORY "")
set(CPACK_INNOSETUP_SETUP_DisableProgramGroupPage ON)
set(CPACK_INNOSETUP_SETUP_PrivilegesRequired "lowest")
set(CPACK_INNOSETUP_SETUP_PrivilegesRequiredOverridesAllowed "dialog")
set(CPACK_INNOSETUP_SETUP_AllowNoIcons ON)
set(CPACK_INNOSETUP_SETUP_UninstallDisplayIcon "{app}/tablica.exe")
set(CPACK_INNOSETUP_SETUP_VersionInfoCompany "${APP_COMPANY}")
set(CPACK_INNOSETUP_SETUP_VersionInfoVersion "${PROJECT_VERSION}")
set(CPACK_INNOSETUP_SETUP_VersionInfoDescription "${APP_NAME}")
set(CPACK_INNOSETUP_SETUP_VersionInfoTextVersion "${PROJECT_VERSION}")
set(CPACK_INNOSETUP_SETUP_VersionInfoProductName "${APP_NAME}")
set(CPACK_INNOSETUP_SETUP_VersionInfoCopyright "Copyright (c) 2026 ${APP_COMPANY}")
set(CPACK_INNOSETUP_SETUP_AppPublisherURL "${CPACK_PACKAGE_HOMEPAGE}")
set(CPACK_INNOSETUP_SETUP_AppSupportURL "${CPACK_PACKAGE_HOMEPAGE}")
set(CPACK_INNOSETUP_SETUP_AppUpdatesURL "${CPACK_PACKAGE_HOMEPAGE}")
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
	set(CPACK_INNOSETUP_ARCHITECTURE "arm64")
endif()

# Haiku HPKG
include(cmake/HaikuPackage.cmake)

set(CPACK_GENERATOR "DEB;RPM;TGZ;ZIP;INNOSETUP;EXTERNAL")
if(NOT HAIKU)
set(CPACK_STRIP_FILES TRUE)
endif()

include(CPack)