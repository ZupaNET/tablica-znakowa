// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#ifndef TABLICA_APP_STATE_H
#define TABLICA_APP_STATE_H

#include <SDL3/SDL.h>

#include "config/config.h"
#include "display/display.h"
#include "input/input.h"
#include "menu/menu.h"
#include "server/server.h"

typedef enum
{
    APP_MODE_DISPLAY,
    APP_MODE_SERVICE
} AppMode;

typedef struct
{
    Config config;

    SDL_Window *window;
    SDL_Renderer *renderer;

    DisplayContext display;

    ServerContext server;

    AppMode mode;

    KeySequence key_sequence;

    ServiceMenu menu;

} AppState;

#endif //TABLICA_APP_STATE_H
