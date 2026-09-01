if(NOT HAIKU)
    return()
endif()

# --------------------------------------
# Definitions
# --------------------------------------

find_program(HAIKU_RC NAMES rc REQUIRED)
find_program(HAIKU_XRES NAMES xres REQUIRED)
message(STATUS "Haiku rc: ${HAIKU_RC}")
message(STATUS "Haiku xres: ${HAIKU_XRES}")
set(HAIKU_RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/haiku")
set(HAIKU_RDEF_IN "${HAIKU_RESOURCE_DIR}/tablica.rdef.in")
set(HAIKU_ICON "${HAIKU_RESOURCE_DIR}/icon.hvif")
set(HAIKU_RDEF "${CMAKE_CURRENT_BINARY_DIR}/tablica.rdef")
set(HAIKU_RSRC "${CMAKE_CURRENT_BINARY_DIR}/tablica.rsrc")

# --------------------------------------
# Generate tablica.rdef
# --------------------------------------

file(READ "${HAIKU_ICON}" TABLICA_HAIKU_ICON)
configure_file("${HAIKU_RDEF_IN}" "${HAIKU_RDEF}" @ONLY)

# --------------------------------------
# Compile resources
# --------------------------------------

add_custom_command(
    OUTPUT "${HAIKU_RSRC}"
    COMMAND "${HAIKU_RC}" -o "${HAIKU_RSRC}" "${HAIKU_RDEF}"
    DEPENDS "${HAIKU_RDEF}"
    COMMENT "Compiling Haiku resources"
    VERBATIM
)

add_custom_target(tablica-haiku-resources DEPENDS "${HAIKU_RSRC}")
add_dependencies(tablica tablica-haiku-resources)

add_custom_command(
    TARGET tablica
    POST_BUILD
    COMMAND "${HAIKU_XRES}" -o "$<TARGET_FILE:tablica>" "${HAIKU_RSRC}"
    COMMENT "Adding Haiku resources to binary"
    VERBATIM
)