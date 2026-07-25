// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#ifndef TABLICA_CONSTANTS_H
#define TABLICA_CONSTANTS_H

#include "SDL3/SDL_keycode.h"

#define CONFIG_FILENAME "tablica.ini"
#define PROGRAM_ICON_PATH "images/icon.bmp"

#define DEFAULT_SERVER_PORT 60023
#define SERVER_MAXIMUM_CLIENTS 64
#define SERVER_BUFFER_SIZE 8192

#define DISPLAY_MAX_LINES 12
#define DISPLAY_PADDING 8
#define DISPLAY_MAX_FONTS 5

static constexpr SDL_Keycode SERVICE_CODE[] =
{
    SDLK_UP,
    SDLK_UP,
    SDLK_DOWN,
    SDLK_DOWN,
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_RETURN
};

static const char* FONT_NAMES[] =
{
    "MiniSet2",
    "MiniForma2",
    "SansSerif",
    "Monocraft",
    "CozetteVector"
};

static constexpr float FONT_MAX_LINES[] =
{
    12.0f,
    10.0f,
    9.0f,
    12.0f,
    12.0f
};

#endif //TABLICA_CONSTANTS_H
