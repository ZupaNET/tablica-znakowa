#ifndef TABLICA_PLATFORM_H
#define TABLICA_PLATFORM_H

#include <SDL3/SDL.h>

void platform_init();
void platform_shutdown();

SDL_IOStream *platform_open_resource(const char* relative_path);
const char *platform_get_resource_path(const char *relative_path);
const char *platform_get_config_path(const char *filename);

#endif //TABLICA_PLATFORM_H
