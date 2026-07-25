// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#ifndef TABLICA_CONFIG_H
#define TABLICA_CONFIG_H

#include <SDL3/SDL.h>

typedef struct
{
    char server_ip[64];
    int server_port;

    int display_padding;
    bool display_fullscreen;

    SDL_Color background_color;
    SDL_Color foreground_color;
} Config;

bool config_load(const char *filename, Config *config);
bool config_save(const char *filename, const Config *config);

#endif //TABLICA_CONFIG_H
