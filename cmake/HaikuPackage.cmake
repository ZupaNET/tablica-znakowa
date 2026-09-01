if(NOT HAIKU)
    return()
endif()

# --------------------------------------
# Tools
# --------------------------------------

find_program(HAIKU_PACKAGE NAMES package REQUIRED)
message(STATUS "Haiku package: ${HAIKU_PACKAGE}")

# --------------------------------------
# Package architecture
# --------------------------------------

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(HAIKU_PACKAGE_ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "riscv64")
    set(HAIKU_PACKAGE_ARCH "riscv64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86")
    set(HAIKU_PACKAGE_ARCH "x86_gcc2")
else()
    message(FATAL_ERROR "Unsupported Haiku architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# --------------------------------------
# Package metadata
# --------------------------------------

set(HAIKU_PACKAGE_COPYRIGHT "Copyright (c) 2026 ${APP_COMPANY}")
set(HAIKU_PACKAGE_LICENSE "GNU GPL v2")

# --------------------------------------
# Generate .PackageInfo
# --------------------------------------

set(HAIKU_PACKAGE_INFO "${CMAKE_CURRENT_BINARY_DIR}/.PackageInfo")
configure_file("${CMAKE_SOURCE_DIR}/resources/haiku/PackageInfo.in" "${HAIKU_PACKAGE_INFO}" @ONLY)

# --------------------------------------
# CPack External
# --------------------------------------

set(CPACK_EXTERNAL_ENABLE_STAGING ON)
set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/HaikuPackageScript.cmake")
configure_file("${CMAKE_CURRENT_LIST_DIR}/HaikuPackageScript.cmake.in" "${CPACK_EXTERNAL_PACKAGE_SCRIPT}" @ONLY)
set(CPACK_PACKAGE_FILE_NAME "${APP_PACKAGE_NAME}-${PROJECT_VERSION}-1-${HAIKU_PACKAGE_ARCH}")