// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#ifndef TABLICA_SERVER_H
#define TABLICA_SERVER_H

#include <SDL3_net/SDL_net.h>

#include "common/constants.h"
#include "display/display.h"
#include "config/config.h"

typedef struct
{
    NET_StreamSocket *socket;
    char buffer[SERVER_MAXIMUM_CLIENTS];
    size_t used;
    char ip[64];
} Client;

typedef struct
{
    NET_Server *server;
    SDL_Thread *server_thread;
    SDL_AtomicInt server_running;
} ServerContext;

bool server_init(const Config *cfg, ServerContext *server);
void server_destroy(ServerContext *server);
void server_parse_client_command(DisplayContext *display, Client *client, const char *line);
int server_thread(void *userdata);

#endif //TABLICA_SERVER_H
