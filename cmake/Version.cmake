set(APP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(APP_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(APP_VERSION_PATCH ${PROJECT_VERSION_PATCH})

math(EXPR APP_VERSION_CODE
        "${APP_VERSION_MAJOR} * 100 +
         ${APP_VERSION_MINOR} * 10 +
         ${APP_VERSION_PATCH}")

set(APP_VERSION_STRING
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)

if(WIN32)

    configure_file(
        ${CMAKE_SOURCE_DIR}/resources/windows/version.rc.in
        ${CMAKE_BINARY_DIR}/version.rc
        @ONLY
    )

elseif(ANDROID)

    configure_file(
        ${CMAKE_SOURCE_DIR}/android/app/build.gradle.in
        ${CMAKE_BINARY_DIR}/build.gradle
        @ONLY
    )

endif()