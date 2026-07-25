if(NOT WIN32)
    return()
endif()

# --------------------------------------
# Try to find existing installation
# --------------------------------------

find_program(INNO_SETUP_COMPILER
        NAMES ISCC.exe
        PATHS
            "$ENV{ProgramFiles}/"
            "$ENV{ProgramFilesx86}/"
            "$ENV{LOCALAPPDATA}/Programs/"

        PATH_SUFFIXES
            "Inno Setup"
            "Inno Setup 5"
            "Inno Setup 6"
            "Inno Setup 7"

        NO_DEFAULT_PATH
)

if(NOT INNO_SETUP_COMPILER)

    find_program(INNO_SETUP_COMPILER
            NAMES ISCC.exe
    )

endif()

if(INNO_SETUP_COMPILER)

    message(STATUS
            "Found Inno Setup compiler:\n"
            "  ${INNO_SETUP_COMPILER}"
    )

    set(INNO_SETUP_COMPILER
            "${INNO_SETUP_COMPILER}"
            CACHE FILEPATH
            "Inno Setup compiler"
    )

    return()

endif()

message(STATUS "Inno Setup not found. Creating installer will likely fail")