#ifndef TABLICA_MENU_H
#define TABLICA_MENU_H

#include "config/config.h"
#include "display/display.h"
#include "input/input.h"

typedef enum
{
    MENU_PAGE_MAIN,
    MENU_PAGE_NETWORK,
    MENU_PAGE_DISPLAY,
    MENU_PAGE_LICENSE
} MenuPage;

typedef enum
{
    EDIT_NONE,
    EDIT_IP,
    EDIT_COLOR,
    EDIT_NUMBER,
    EDIT_BOOL
} EditMode;

typedef enum
{
    MENU_FIELD_IP,
    MENU_FIELD_PORT,
    MENU_FIELD_PADDING,
    MENU_FIELD_FULLSCREEN,
    MENU_FIELD_BACKGROUND,
    MENU_FIELD_FOREGROUND

} MenuField;

typedef struct
{
    const char *caption;
    MenuField field;
    EditMode edit_mode;
} MenuItem;

typedef struct
{
    bool hideOverlay;

    MenuPage page;
    int page_item_count;

    int selected_item;
    EditMode edit_mode;

    Config temporary_config;

    char input_buffer[64];
    int input_length;
} ServiceMenu;

void service_menu_init(ServiceMenu *menu, DisplayContext *display, const Config *config);
void service_menu_handle(void *state, InputEvent ev);
void service_menu_render(ServiceMenu *menu, DisplayContext *display);

#endif //TABLICA_MENU_H
