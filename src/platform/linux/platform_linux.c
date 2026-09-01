// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <dev@zupanet.pl>
 */

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../platform.h"
#include "common/constants.h"
#include "state/app_state.h"

static char resource_path[1024];
static char config_path[1024];

static SDL_Surface* load_image(const char *filename)
{
    int width, height, channels;

    unsigned char *pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels)
        return nullptr;

    SDL_Surface *surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (!surface)
    {
        stbi_image_free(pixels);
        return nullptr;
    }

    SDL_LockSurface(surface);

    for (int y = 0; y < height; y++)
    {
        SDL_memcpy(
            (Uint8*)surface->pixels + y * surface->pitch,
            pixels + y * width * 4,
            width * 4
        );
    }

    SDL_UnlockSurface(surface);

    stbi_image_free(pixels);

    return surface;
}

void platform_init()
{
}

void platform_sdl_postinit(void *data)
{
    AppState *app = (AppState *)data;
    SDL_Surface* icon = load_image(platform_get_resource_path(PROGRAM_ICON_PATH));
    if (icon)
    {
        SDL_SetWindowIcon(app->window, icon);
        SDL_DestroySurface(icon);
    }
    else
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Nie można poprawnie załadować ikony programu: %s\n", SDL_GetError());
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
        "/usr/local/share/"APP_PACKAGE_NAME"/",
        relative_path
    );

    if (file_exists(resource_path))
        return resource_path;

    // In /usr/share/tablica/
    SDL_snprintf(
        resource_path,
        sizeof(resource_path),
        "%s%s",
        "/usr/share/"APP_PACKAGE_NAME"/",
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