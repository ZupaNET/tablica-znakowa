set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

find_program(CMAKE_C_COMPILER
        NAMES x86_64-w64-mingw32-clang
        x86_64-w64-mingw32-gcc
)

find_program(CMAKE_CXX_COMPILER
        NAMES x86_64-w64-mingw32-clang++
        x86_64-w64-mingw32-g++
)

find_program(CMAKE_RC_COMPILER
        NAMES x86_64-w64-mingw32-windres
)

if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "Failed to find C compiler.")
endif()

if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "Failed to find C++ compiler.")
endif()

if(NOT CMAKE_RC_COMPILER)
    message(FATAL_ERROR "Failed to find resource compiler.")
endif()