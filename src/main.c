// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <dev@zupanet.pl>
 */

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "config/config.h"
#include "display/display.h"
#include "input/input.h"
#include "menu/menu.h"
#include "platform/platform.h"
#include "server/server.h"
#include "state/app_state.h"

static void print_header()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"\n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"          ┏┳┓╻┏ ┏━╸╻┏━┓      \n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"       ┃┏┛┃┃┃┣┻┓┣╸ ┃┣━┫      \n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"       ┗┛ ╹ ╹╹ ╹┗━╸╹╹ ╹      \n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"\n");
}

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

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{

    AppState *app = SDL_calloc(1, sizeof(AppState));
    if (!app)
        return SDL_APP_FAILURE;
    *appstate = app;

    print_header();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Ładowanie aplikacji " APP_NAME);
    platform_init();
    input_key_sequence_init(&app->key_sequence, SERVICE_CODE, SDL_arraysize(SERVICE_CODE));
    if (!config_load(platform_get_config_path(CONFIG_FILENAME), &app->config))
        return SDL_APP_FAILURE;

#ifndef SDL_PLATFORM_ANDROID
    if (!SDL_CreateWindowAndRenderer(APP_NAME " " APP_VERSION_STRING, 1152 + app->config.display_padding, 768 - app->config.display_padding, app->config.display_fullscreen ? (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS) : SDL_WINDOW_RESIZABLE, &app->window, &app->renderer))
#else
    if (!SDL_CreateWindowAndRenderer(APP_NAME " " APP_VERSION_STRING, 0, 0, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS, &app->window, &app->renderer))
#endif
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można utworzyć okna i silnika renderującego: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
#ifndef SDL_PLATFORM_WINDOWS
    SDL_Surface* icon = load_image(platform_get_resource_path(PROGRAM_ICON_PATH));
    if (icon)
    {
        SDL_SetWindowIcon(app->window, icon);
        SDL_DestroySurface(icon);
    }
    else
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Nie można poprawnie załadować ikony programu: %s\n", SDL_GetError());
#endif

    if (!display_init(&app->display))
        return SDL_APP_FAILURE;

    if (!server_init(&app->config, &app->server))
        return SDL_APP_FAILURE;
    app->server.server_thread = SDL_CreateThread(server_thread, "TablicaZnakowaServer", app);


    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Aplikacja " APP_NAME " jest gotowa do działania!");
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *app = (AppState *)appstate;

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;


        case SDL_EVENT_KEY_DOWN:
            if (event->key.repeat)
                return SDL_APP_CONTINUE;

            if (app->mode == APP_MODE_SERVICE)
            {
                InputEvent ev = input_translate(event->key.key);
                service_menu_handle(app, ev);
                return SDL_APP_CONTINUE;
            }

            if (input_key_sequence_check(&app->key_sequence, event->key.key))
            {
                app->mode = APP_MODE_SERVICE;
                service_menu_init(&app->menu, &app->display, &app->config);
                SDL_Log("INFO: Włączono tryb serwisowy!");
            }

            switch (event->key.key)
            {
                case SDLK_ESCAPE:
                case SDLK_AC_BACK:
                    return SDL_APP_SUCCESS;

                default:
                    break;
            }

            break;

        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *app = (AppState *)appstate;

    /* Clear screen */
    SDL_SetRenderDrawColor(app->renderer, app->config.background_color.r, app->config.background_color.g, app->config.background_color.b, 255);
    SDL_RenderClear(app->renderer);

    /* Get buffer pointer copy */
    SDL_LockMutex(app->display.mutex);
    DisplayState *state = app->display.front_buffer;
    SDL_UnlockMutex(app->display.mutex);

    if (!state || !state -> font)
    {
        SDL_RenderPresent(app->renderer);
        return SDL_APP_CONTINUE;
    }

    int screen_w, screen_h;
    SDL_GetRenderOutputSize(app->renderer, &screen_w, &screen_h);

    const float factor = (float)state->brightness / 4.0f;
    const SDL_Color base = {app->config.foreground_color.r, app->config.foreground_color.g, app->config.foreground_color.b, 255};
    const SDL_Color color =
    {
        .r = (Uint8)((float)base.r * factor),
        .g = (Uint8)((float)base.g * factor),
        .b = (Uint8)((float)base.b * factor),
        .a = 255
    };

    float y = (float)app->config.display_padding;
    float scale = min((float)screen_h / DISPLAY_TRUE_HEIGHT, (float)screen_w / DISPLAY_TRUE_WIDTH);

    TTF_SetFontSizeDPI(state->font, state->font_size * scale, 72, 72);

    for (int i = 0; i < DISPLAY_MAX_LINES; i++)
    {

        const char *text = state->lines[i];
        if (!text || !text[0])
        {
            y += state->line_height * scale;
            continue;
        }

        SDL_Surface *surface = TTF_RenderText_Blended(state->font, text, 0, color);
        if (!surface)
            continue;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(app->renderer, surface);
        if (!texture)
        {
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_FRect dst = {
            .x = (float)app->config.display_padding,
            .y = y,
            .w = (float)surface->w,
            .h = (float)surface->h
        };

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);

        SDL_RenderTexture(app->renderer, texture, nullptr, &dst);

        y += state->line_height * scale;

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

    SDL_RenderPresent(app->renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Zamykam aplikację!");
    AppState *state = (AppState *)appstate;

    server_destroy(&state->server);
    display_destroy(&state->display);
}