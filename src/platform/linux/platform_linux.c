// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <dev@zupanet.pl>
 */

#ifdef PLATFORM_LINUX

#include "../platform.h"

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

static bool file_exists(const char *path)
{
    SDL_IOStream *io = SDL_IOFromFile(path, "rb");
    if (!io) return false;

    SDL_CloseIO(io);
    return true;
}

const char *platform_get_resource_path(const char *relative_path)
{
    const char *base = SDL_GetBasePath();
    if (!base)
        return nullptr;

    // Besides app binary in ./resources/
    SDL_snprintf(
        resource_path,
        sizeof(resource_path),
        "%sresources/%s",
        base,
        relative_path
    );

    if (file_exists(resource_path))
        return resource_path;

    // In /usr/local/share/tablica/
    SDL_snprintf(
        resource_path,
        sizeof(resource_path),
        "%s%s",
        "/usr/local/share/tablica/",
        relative_path
    );

    if (file_exists(resource_path))
        return resource_path;

    // In /usr/share/tablica/
    SDL_snprintf(
        resource_path,
        sizeof(resource_path),
        "%s%s",
        "/usr/share/tablica/",
        relative_path
    );

    if (file_exists(resource_path))
        return resource_path;

    return nullptr;
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