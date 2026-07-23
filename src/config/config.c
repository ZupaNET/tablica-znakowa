#include "config.h"
#include "common/constants.h"
#include <ini.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdbool.h>

static bool parse_hex_color(const char *str, SDL_Color *color)
{
    unsigned int r, g, b;

    if (sscanf(str, "#%2x%2x%2x", &r, &g, &b) != 3)
        return false;

    color->r = (Uint8)r;
    color->g = (Uint8)g;
    color->b = (Uint8)b;
    color->a = 255;

    return true;
}

static int handler(void *user, const char* section, const char* name, const char* value)
{
    Config *config = (Config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("server", "ip"))
    {
        SDL_strlcpy(config->server_ip, value, sizeof(config->server_ip));
    }
    else if (MATCH("server", "port"))
    {
        config->server_port = strtol(value, NULL, 10);
    }
    else if (MATCH("display", "padding"))
    {
        config->display_padding = strtol(value, NULL, 10);
    }
    else if (MATCH("display", "fullscreen"))
    {
        config->display_fullscreen = strtol(value, NULL, 10);
    }
    else if (MATCH("display", "background"))
    {
        parse_hex_color(value, &config->background_color);
    }
    else if (MATCH("display", "foreground"))
    {
        parse_hex_color(value, &config->foreground_color);
    }
    else
    {
        return 0;
    }

    return 1;
}

static void config_populate_with_defaults(Config *config)
{
    SDL_strlcpy(config->server_ip, "0.0.0.0", sizeof(config->server_ip)-1);
    config->server_port = DEFAULT_SERVER_PORT;
    config->display_padding = DISPLAY_PADDING;
#ifndef PLATFORM_ANDROID
    config->display_fullscreen = 0;
#else
    config->display_fullscreen = 1;
#endif
    config->background_color.r = 0; config->background_color.g = 0; config->background_color.b = 0;
    config->foreground_color.r = 255; config->foreground_color.g = 0; config->foreground_color.b = 0;
}

bool config_load(const char *filename, Config *config)
{
    if (ini_parse(filename, handler, config) < 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,"Nie można załadować pliku konfiguracyjnego! Wczytuję ustawienia domyślne\n");
        config_populate_with_defaults(config);
        return config_save(filename, config);
    }

    return true;
}

bool config_save(const char *filename, const Config *config)
{
    SDL_IOStream *file = SDL_IOFromFile(filename, "wb");
    if (!file)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można otworzyć pliku konfiguracyjnego do zapisu: %s\n", SDL_GetError());
        return false;
    }

    const char *fmt =
        "[server]\n"
        "ip=%s\n"
        "port=%d\n"
        "\n"
        "[display]\n"
        "padding=%d\n"
        "fullscreen=%d\n"
        "background=#%02x%02x%02x\n"
        "foreground=#%02x%02x%02x\n";

    char buffer[1024];
    SDL_snprintf(buffer, sizeof(buffer),
        fmt,
        config->server_ip,
        config->server_port,
        config->display_padding,
        config->display_fullscreen,
        config->background_color.r, config->background_color.g, config->background_color.b,
        config->foreground_color.r, config->foreground_color.g, config->foreground_color.b);

    size_t len = SDL_strlen(buffer);

    if (SDL_WriteIO(file, buffer, len) != len)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie zapisać pliku konfiguracyjnego: %s\n", SDL_GetError());
        SDL_CloseIO(file);
        return false;
    }

    SDL_CloseIO(file);
    return true;
}