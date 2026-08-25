// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <dev@zupanet.pl>
 */

#ifdef PLATFORM_WINDOWS

#include <SDL3/SDL_stdinc.h>
#include <windows.h>
#include "../platform.h"

static char resource_path[1024];
static char config_path[1024];

static void detach_console_if_owned()
{
    HWND hwnd = GetConsoleWindow();

    if (!hwnd)
        return;

    DWORD processes[16];
    DWORD count = GetConsoleProcessList(
        processes,
        SDL_arraysize(processes));

    if (count == 1)
    {
        FreeConsole();
    }
}

void platform_init()
{
    detach_console_if_owned();
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
        "%sresources\\%s",
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