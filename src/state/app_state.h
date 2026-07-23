#ifndef TABLICA_APP_STATE_H
#define TABLICA_APP_STATE_H

#include <SDL3/SDL.h>

#include "config/config.h"
#include "display/display.h"
#include "server/server.h"

typedef struct
{
    Config config;

    SDL_Window *window;
    SDL_Renderer *renderer;

    DisplayContext display;

    ServerContext server;

} AppState;

#endif //TABLICA_APP_STATE_H
