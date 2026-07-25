#include "display.h"
#include "common/constants.h"
#include "platform/platform.h"



static void display_buffer_init(DisplayState *buffer, TTF_Font *default_font)
{
    buffer->brightness = 4;
    buffer->font = default_font;
    buffer->maximum_lines = DISPLAY_MAX_LINES;
    for (int i = 1; i < DISPLAY_MAX_LINES; i++)
    {
        buffer->lines[i] = (char*)SDL_malloc(1);
        buffer->lines[i][0] = '\0';
    }
    buffer->lines[0] = (char*)SDL_malloc(2);
    buffer->lines[0][0] = '.'; buffer->lines[0][1] = '\0';
}

static bool display_fonts_init(DisplayContext *display)
{
    static char resource_name[1024];
    int i;
    for (i = 0; i < SDL_arraysize(FONT_NAMES); i++)
    {
        SDL_snprintf(resource_name, 1024, "fonts/%s.ttf", FONT_NAMES[i]);
        display->fonts[i] = TTF_OpenFontIO(platform_open_resource(resource_name), true, 64.0f);
        if (!display->fonts[i])
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można załadować czcionki %s: %s\n", FONT_NAMES[i], SDL_GetError());
            return false;
        }
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Załadowano %d czcionek", i);

    return true;
}

bool display_init(DisplayContext *display)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Ładowanie podsystemu ekranu");
    if (!TTF_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można załadować SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Ładowanie czcionek");
    if (!display_fonts_init(display))
        return false;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Inicjalizacja buforów");
    display_buffer_init(&display->buffer1, display->fonts[0]);
    display_buffer_init(&display->buffer2, display->fonts[0]);
    display->mutex = SDL_CreateMutex();
    display->back_buffer = &display->buffer1;
    display->front_buffer = &display->buffer2;

    return true;
}

static void display_buffer_destroy(const DisplayState *buffer)
{
    for (int i = 1; i < DISPLAY_MAX_LINES; i++)
    {
        SDL_free(buffer->lines[i]);
    }
}

static void display_fonts_destroy(const DisplayContext *display)
{
    for (int i = 0; i < DISPLAY_MAX_FONTS; i ++)
    {
        if (display->fonts[i] != nullptr)
            TTF_CloseFont(display->fonts[i]);
    }
}

void display_destroy(const DisplayContext *display)
{
    display_buffer_destroy(&display->buffer1);
    display_buffer_destroy(&display->buffer2);
    SDL_DestroyMutex(display->mutex);
    display_fonts_destroy(display);
    TTF_Quit();
}

void display_swap_buffers(DisplayContext *display)
{
    SDL_LockMutex(display->mutex);

    DisplayState* tmp = display->front_buffer;

    display->front_buffer = display->back_buffer;
    display->back_buffer = tmp;

    SDL_UnlockMutex(display->mutex);
}

void display_commit_brightness(const DisplayContext *display)
{
    SDL_LockMutex(display->mutex);

    display->front_buffer->brightness = display->back_buffer->brightness;

    SDL_UnlockMutex(display->mutex);
}

void display_draw_line_on_back_buffer(const DisplayContext *display, const int line_number, const char *line)
{
    if (line_number >= DISPLAY_MAX_LINES)
        return;

    const size_t line_length = SDL_strlen(line);

    char *new_line = SDL_realloc(display->back_buffer->lines[line_number],line_length + 1);
    if (!new_line)
        return;
    display->back_buffer->lines[line_number] = new_line;

    SDL_strlcpy(display->back_buffer->lines[line_number], line, line_length + 1);
}

void display_change_font(const DisplayContext *display, int font_number)
{
    if (font_number >= DISPLAY_MAX_FONTS)
        return;

    if (display->fonts[font_number] == nullptr)
        return;

    display->back_buffer->font = display->fonts[font_number];
    display->back_buffer->maximum_lines = FONT_MAX_LINES[font_number];
}

void display_clear(DisplayContext *display)
{
    for (int i = 0; i < DISPLAY_MAX_LINES; i++)
    {
        display_draw_line_on_back_buffer(display, i, "");
    }

    display_swap_buffers(display);

    for (int i = 0; i < DISPLAY_MAX_LINES; i++)
    {
        display_draw_line_on_back_buffer(display, i, "");
    }
}