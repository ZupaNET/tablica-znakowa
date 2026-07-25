if(NOT ANDROID)
    return()
endif()

# ------------------------------------------------------------------------------
# Detect NDK prebuilt directory
# ------------------------------------------------------------------------------

get_filename_component(_ANDROID_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
get_filename_component(ANDROID_PREBUILT_DIR "${_ANDROID_BIN_DIR}" DIRECTORY)

# ------------------------------------------------------------------------------
# ABI -> sysroot triple
# ------------------------------------------------------------------------------

if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(ANDROID_SYSROOT_TRIPLE "aarch64-linux-android")
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(ANDROID_SYSROOT_TRIPLE "arm-linux-androideabi")
elseif(ANDROID_ABI STREQUAL "x86")
    set(ANDROID_SYSROOT_TRIPLE "i686-linux-android")
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(ANDROID_SYSROOT_TRIPLE "x86_64-linux-android")
else()
    message(FATAL_ERROR "Unsupported Android ABI: ${ANDROID_ABI}")
endif()

# ------------------------------------------------------------------------------
# Runtime libraries
# ------------------------------------------------------------------------------

set(ANDROID_LIBCXX_SHARED
    "${ANDROID_PREBUILT_DIR}/sysroot/usr/lib/${ANDROID_SYSROOT_TRIPLE}/libc++_shared.so")

if(NOT EXISTS "${ANDROID_LIBCXX_SHARED}")
    message(FATAL_ERROR
        "libc++_shared.so not found:\n${ANDROID_LIBCXX_SHARED}")
endif()

# ------------------------------------------------------------------------------
# Export
# ------------------------------------------------------------------------------

set(ANDROID_PREBUILT_DIR     "${ANDROID_PREBUILT_DIR}"     )
set(ANDROID_SYSROOT_TRIPLE   "${ANDROID_SYSROOT_TRIPLE}"   )
set(ANDROID_LIBCXX_SHARED    "${ANDROID_LIBCXX_SHARED}"    )