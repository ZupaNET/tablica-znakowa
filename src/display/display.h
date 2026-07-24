#ifndef TABLICA_DISPLAY_H
#define TABLICA_DISPLAY_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "common/constants.h"

typedef struct
{
    char *lines[DISPLAY_MAX_LINES];
    unsigned int brightness;
    TTF_Font *font;
    float maximum_lines;
} DisplayState;

typedef struct
{
    TTF_Font *fonts[DISPLAY_MAX_FONTS];

    DisplayState buffer1;
    DisplayState buffer2;

    DisplayState *front_buffer;
    DisplayState *back_buffer;
    SDL_Mutex *mutex;
} DisplayContext;

bool display_init(DisplayContext *display);
void display_destroy(DisplayContext *display);
void display_swap_buffers(DisplayContext *display);
void display_commit_brightness(const DisplayContext *display);
void display_draw_line_on_back_buffer(const DisplayContext *display, int line_number, const char *line);
void display_change_font(const DisplayContext *display, int font_number);
void display_clear(DisplayContext *display);

#endif //TABLICA_DISPLAY_H
