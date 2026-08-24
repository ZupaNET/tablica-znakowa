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
#define PROGRAM_ICON_PATH "images/icon.png"

#define DEFAULT_SERVER_PORT 60023
#define SERVER_MAXIMUM_CLIENTS 64
#define SERVER_BUFFER_SIZE 8192

#define DISPLAY_MAX_LINES 14
#define DISPLAY_PADDING 8
#define DISPLAY_MAX_FONTS 5
#define DISPLAY_TRUE_WIDTH 192
#define DISPLAY_TRUE_HEIGHT 128

static const SDL_Keycode SERVICE_CODE[] =
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

static const float FONT_SIZES[] =
{
    8.0f,   // MiniSet2
    8.0f,   // MiniForma2
    11.7f,  // FreeSans is not necessary fully metric-correct with MSSans, should be 12
    8.0f,   // Monocraft
    8.0f    // CozetteVector
};

static const float FONT_LINE_HEIGHTS[] =
{
    9.0f,   // MiniSet2
    11.0f,  // MiniForma2
    14.0f,  // FreeSans / MSSans
    10.0f,   // Monocraft
    10.0f,   // CozetteVector
};

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

#endif //TABLICA_CONSTANTS_H
