#include "../platform.h"

#ifdef PLATFORM_ANDROID

static char resource_path[1024];
static char config_path[1024];

void platform_init()
{
}

void platform_shutdown()
{
}

SDL_IOStream *platform_open_resource(const char *relative_path)
{
    return SDL_IOFromFile(
        platform_get_resource_path(relative_path),
        "rb"
    );
}

const char *platform_get_resource_path(const char *relative_path)
{
    const char *base = SDL_GetBasePath();
    if (!base)
        return nullptr;

    SDL_snprintf(
        resource_path,
        sizeof(resource_path),
        "%s%s",
        base,
        relative_path
    );

    return resource_path;
}

const char *platform_get_config_path(const char *filename)
{
    char *pref = SDL_GetPrefPath(
        APP_COMPANY,
        APP_NAME
    );

    if (!pref)
        return nullptr;

    SDL_snprintf(
        config_path,
        sizeof(config_path),
        "%s%s",
        pref,
        filename
    );

    SDL_free(pref);

    return config_path;
}

#endif