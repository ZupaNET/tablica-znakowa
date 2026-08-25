// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#include "menu.h"
#include "platform/platform.h"
#include "state/app_state.h"

static const MenuItem network_items[] =
{
    { "ADRES IP", MENU_FIELD_IP, EDIT_IP },
    { "PORT", MENU_FIELD_PORT, EDIT_NUMBER },
};

static const MenuItem display_items[] =
{
    { "ODSTĘP OD EKRANU", MENU_FIELD_PADDING, EDIT_NUMBER },
    { "PEŁEN EKRAN", MENU_FIELD_FULLSCREEN, EDIT_BOOL },
    { "KOLOR TŁA", MENU_FIELD_BACKGROUND, EDIT_COLOR },
    { "KOLOR CZCIONKI", MENU_FIELD_FOREGROUND, EDIT_COLOR },
};

void service_menu_init(ServiceMenu *menu, DisplayContext *display, const Config *config)
{
    menu->hideOverlay = false;
    menu->page = MENU_PAGE_MAIN;
    menu->page_item_count = 4;
    menu->selected_item = 0;
    menu->edit_mode = EDIT_NONE;
    menu->temporary_config = *config;
    SDL_zero(menu->input_buffer);
    menu->input_length = 0;

    service_menu_render(menu, display);
}

static void service_menu_draw_option(const DisplayContext *display, bool selected, int line, const char *fmt, ...)
{
    va_list args;
    char buffer[128];
    SDL_zero(buffer);
    va_start(args, fmt);

    buffer[0] = selected? '*' : ' '; buffer[1] = ' ';
    SDL_vsnprintf(buffer+2, sizeof(buffer)-2, fmt, args);
    display_draw_line_on_back_buffer(display, line, buffer);

    va_end(args);
}

static bool service_menu_validate_ip(const char *ip)
{
    int parts = 0;
    int value = 0;
    int digits = 0;

    for (const char *p = ip; ; p++)
    {
        char c = *p;

        if (c >= '0' && c <= '9')
        {
            value = value * 10 + (c - '0');
            digits++;

            if (value > 255)
                return false;
        }
        else if (c == '.' || c == '\0')
        {
            if (digits == 0)
                return false;

            parts++;

            if (parts > 4)
                return false;

            value = 0;
            digits = 0;

            if (c == '\0')
                break;
        }
        else
        {
            return false;
        }
    }

    return parts == 4;
}

static void service_menu_save_item(ServiceMenu *menu)
{
    switch (menu->edit_mode)
    {
        case EDIT_IP:
            switch (menu->page)
            {
                case MENU_PAGE_NETWORK:
                    switch (menu->selected_item)
                    {
                        case 0:
                            SDL_strlcpy(menu->temporary_config.server_ip, menu->input_buffer, SDL_arraysize(menu->temporary_config.server_ip));
                            SDL_Log("INFO: Zmieniono adres IP: %s", menu->temporary_config.server_ip);
                            break;
                        default: break;
                    }
                    break;
                //case MENU_PAGE_DISPLAY:
                //    break;
                default: break;
            }
            break;
        case EDIT_NUMBER:
            switch (menu->page)
            {
                case MENU_PAGE_NETWORK:
                    switch (menu->selected_item)
                    {
                        case 1:
                            menu->temporary_config.server_port = SDL_strtol(menu->input_buffer, nullptr, 10);
                            SDL_Log("INFO: Zmieniono port: %d", menu->temporary_config.server_port);
                            break;
                        default: break;
                    }
                    break;
                case MENU_PAGE_DISPLAY:
                    switch (menu->selected_item)
                    {
                        case 0:
                            menu->temporary_config.display_padding = SDL_strtol(menu->input_buffer, nullptr, 10);
                            SDL_Log("INFO: Zmieniono padding ekranu: %d", menu->temporary_config.display_padding);
                            break;
                        default: break;
                    }
                    break;
                default: break;
            }
            break;
        case EDIT_COLOR:
            switch (menu->page)
            {
                case MENU_PAGE_NETWORK: break;
                case MENU_PAGE_DISPLAY:
                    switch (menu->selected_item)
                    {
                        case 2:
                            SDL_sscanf(menu->input_buffer, "%hhu,%hhu,%hhu", &menu->temporary_config.background_color.r,
                                &menu->temporary_config.background_color.g, &menu->temporary_config.background_color.b);
                            SDL_Log("INFO: Ustawiono kolor tła: #%02x%02x%02x", menu->temporary_config.background_color.r,
                                menu->temporary_config.background_color.g, menu->temporary_config.background_color.b);
                            break;
                        case 3:
                            SDL_sscanf(menu->input_buffer, "%hhu,%hhu,%hhu", &menu->temporary_config.foreground_color.r,
                                &menu->temporary_config.foreground_color.g, &menu->temporary_config.foreground_color.b);
                            SDL_Log("INFO: Ustawiono kolor czcionki: #%02x%02x%02x", menu->temporary_config.foreground_color.r,
                                menu->temporary_config.foreground_color.g, menu->temporary_config.foreground_color.b);
                            break;
                        default: break;
                    }
                    break;
                default: break;
            }
            break;
        case EDIT_BOOL:
            switch (menu->page)
            {
                case MENU_PAGE_NETWORK: break;
                case MENU_PAGE_DISPLAY:
                    switch (menu->selected_item)
                    {
                        case 1:
                            menu->temporary_config.display_fullscreen = !menu->temporary_config.display_fullscreen;
                            SDL_Log("INFO: %s tryb pełnoekranowy", menu->temporary_config.display_fullscreen ? "Włączono" : "Wyłączono");
                            break;
                        default: break;
                    }
                    break;
                default: break;
            }
            break;
        default: break;
    }
}

static void service_menu_edit_ip(ServiceMenu *menu, InputEvent ev)
{
    if (ev.action == INPUT_DIGIT && ev.digit >= 0 && ev.digit <= 9)
    {
        if (menu->input_length < SDL_arraysize(menu->input_buffer)-1)
        {
            menu->input_buffer[menu->input_length++] = (char)('0' + ev.digit);
            menu->input_buffer[menu->input_length] = '\0';
        }

        return;
    }

    if (ev.action == INPUT_RIGHT || ev.action == INPUT_DELIMITER)
    {
        if (menu->input_length < SDL_arraysize(menu->input_buffer)-1)
        {
            if (menu->input_length > 0 && menu->input_buffer[menu->input_length-1] != '.' && !service_menu_validate_ip(menu->input_buffer))
            {
                menu->input_buffer[menu->input_length++] = '.';
                menu->input_buffer[menu->input_length] = '\0';
            }
        }

        return;
    }

    if (ev.action == INPUT_BACK)
    {
        if (menu->input_length > 0)
        {
            menu->input_length--;
            menu->input_buffer[menu->input_length] = '\0';
        }
        else
        {
            menu->edit_mode = EDIT_NONE;
        }

        return;
    }

    if (ev.action == INPUT_OK && service_menu_validate_ip(menu->input_buffer))
    {
        service_menu_save_item(menu);
        menu->edit_mode = EDIT_NONE;
        return;
    }
}

static void service_menu_edit_number(ServiceMenu *menu, InputEvent ev)
{
    if (ev.action == INPUT_DIGIT && ev.digit >= 0 && ev.digit <= 9)
    {
        if (menu->input_length < SDL_arraysize(menu->input_buffer)-1)
        {
            menu->input_buffer[menu->input_length++] = (char)('0' + ev.digit);
            menu->input_buffer[menu->input_length] = '\0';
        }

        return;
    }

    if (ev.action == INPUT_BACK)
    {
        if (menu->input_length > 0)
        {
            menu->input_length--;
            menu->input_buffer[menu->input_length] = '\0';
        }
        else
        {
            menu->edit_mode = EDIT_NONE;
        }

        return;
    }

    if (ev.action == INPUT_OK)
    {
        service_menu_save_item(menu);
        menu->edit_mode = EDIT_NONE;
        return;
    }
}

static bool service_menu_validate_color(const char *value)
{
    int components = 0;
    int number = 0;
    int digits = 0;

    for (const char *p = value; ; p++)
    {
        char c = *p;

        if (c >= '0' && c <= '9')
        {
            number = number * 10 + (c - '0');
            digits++;

            if (number > 255)
                return false;
        }
        else if (c == ',' || c == '\0')
        {
            if (digits == 0)
                return false;

            components++;

            if (components > 3)
                return false;

            number = 0;
            digits = 0;

            if (c == '\0')
                break;
        }
        else
        {
            return false;
        }
    }

    return components == 3;
}

static void service_menu_edit_color(ServiceMenu *menu, InputEvent ev)
{
    if (ev.action == INPUT_DIGIT && ev.digit >= 0 && ev.digit <= 9)
    {
        if (menu->input_length < SDL_arraysize(menu->input_buffer)-1)
        {
            menu->input_buffer[menu->input_length++] = (char)('0' + ev.digit);
            menu->input_buffer[menu->input_length] = '\0';
        }

        return;
    }

    if (ev.action == INPUT_RIGHT || ev.action == INPUT_DELIMITER)
    {
        if (menu->input_length < SDL_arraysize(menu->input_buffer)-1)
        {
            if (menu->input_length > 0 && menu->input_buffer[menu->input_length-1] != ',')
            {
                int commas = 0;

                for (int i = 0; i < menu->input_length; i++)
                {
                    if (menu->input_buffer[i] == ',')
                        commas++;
                }

                if (commas < 2)
                {
                    menu->input_buffer[menu->input_length++] = ',';
                    menu->input_buffer[menu->input_length] = '\0';
                }
            }
        }

        return;
    }

    if (ev.action == INPUT_BACK)
    {
        if (menu->input_length > 0)
        {
            menu->input_length--;
            menu->input_buffer[menu->input_length] = '\0';
        }
        else
        {
            menu->edit_mode = EDIT_NONE;
        }

        return;
    }

    if (ev.action == INPUT_OK && service_menu_validate_color(menu->input_buffer))
    {
        service_menu_save_item(menu);
        menu->edit_mode = EDIT_NONE;
    }
}

static void service_menu_edit_bool(ServiceMenu *menu, InputEvent ev)
{
    if (ev.action == INPUT_BACK)
    {
        menu->edit_mode = EDIT_NONE;
        return;
    }

    if (ev.action == INPUT_OK)
    {
        service_menu_save_item(menu);
        menu->edit_mode = EDIT_NONE;
        return;
    }
}

static void service_menu_edit_handle(ServiceMenu *menu, InputEvent ev)
{
    switch (menu->edit_mode)
    {
        case EDIT_IP:
            service_menu_edit_ip(menu, ev);
            break;
        case EDIT_NUMBER:
            service_menu_edit_number(menu, ev);
            break;
        case EDIT_COLOR:
            service_menu_edit_color(menu, ev);
            break;
        case EDIT_BOOL:
            service_menu_edit_bool(menu, ev);
            break;
        default: break;
    }
}

static void service_menu_move(ServiceMenu *menu, InputEvent ev)
{
    if (ev.action == INPUT_UP && menu->selected_item > 0)
        menu->selected_item--;

    if (ev.action == INPUT_DOWN && menu->selected_item < menu->page_item_count - 1)
        menu->selected_item++;
}

static void service_menu_enter(ServiceMenu *menu, AppMode *mode, DisplayContext *display, int *exit)
{
    *exit = 0;
    switch (menu->page)
    {
        case MENU_PAGE_MAIN:
            switch (menu->selected_item)
            {
                case 0: menu->page = MENU_PAGE_NETWORK; menu->page_item_count = SDL_arraysize(network_items); menu->selected_item = 0; menu->edit_mode = EDIT_NONE; menu->hideOverlay = false; break;
                case 1: menu->page = MENU_PAGE_DISPLAY; menu->page_item_count = SDL_arraysize(display_items); menu->selected_item = 0; menu->edit_mode = EDIT_NONE; menu->hideOverlay = false; break;
                case 2: menu->page = MENU_PAGE_LICENSE; menu->page_item_count = 0; menu->selected_item = 0; menu->edit_mode = EDIT_NONE; menu->hideOverlay = true; break;
                case 3:
                    config_save(platform_get_config_path(CONFIG_FILENAME), &menu->temporary_config);
                    display_clear(display);
                    *exit = 1;
                    service_menu_draw_option(display, true, 0, "Zrestartuj tablicę,");
                    service_menu_draw_option(display, true, 1, "aby wczytać nową konfigurację.");
                    display_swap_buffers(display);
                    *mode = APP_MODE_DISPLAY;
                    SDL_Log("INFO: Zapisano konfigurację i opuszczono tryb serwisowy!"); break;
                default: break;
            }
            break;
        case MENU_PAGE_NETWORK:
            SDL_zero(menu->input_buffer); menu->input_length = 0;
            menu->edit_mode = network_items[menu->selected_item].edit_mode;
            break;
        case MENU_PAGE_DISPLAY:
            SDL_zero(menu->input_buffer); menu->input_length = 0;
            menu->edit_mode = display_items[menu->selected_item].edit_mode;
            break;
        case MENU_PAGE_LICENSE:
            break;
    }
}

static void service_menu_back(ServiceMenu *menu, AppMode *mode, DisplayContext *display, int *exit)
{
    *exit = 0;
    switch (menu->page)
    {
        case MENU_PAGE_MAIN:
            display_clear(display);
            *exit = 1;
            *mode = APP_MODE_DISPLAY;
            SDL_Log("INFO: Opuszczono tryb serwisowy!");
            break;
        case MENU_PAGE_NETWORK:
            menu->page = MENU_PAGE_MAIN;
            menu->selected_item = 0;
            menu->page_item_count = 4;
            menu->hideOverlay = false;
            break;
        case MENU_PAGE_DISPLAY:
            menu->page = MENU_PAGE_MAIN;
            menu->selected_item = 1;
            menu->page_item_count = 4;
            menu->hideOverlay = false;
            break;
        case MENU_PAGE_LICENSE:
            menu->page = MENU_PAGE_MAIN;
            menu->selected_item = 2;
            menu->page_item_count = 4;
            menu->hideOverlay = false;
            break;
    }
}

void service_menu_handle(void *state, InputEvent ev)
{
    int exit;
    AppState *app = (AppState *)state;
    ServiceMenu *menu = &app->menu;

    if (menu->edit_mode != EDIT_NONE)
    {
        service_menu_edit_handle(menu, ev);
        service_menu_render(menu, &app->display);
        return;
    }

    switch (ev.action)
    {
        case INPUT_DOWN:
        case INPUT_UP:
            service_menu_move(menu, ev);
            break;
        case INPUT_OK:
            service_menu_enter(menu, &app->mode, &app->display, &exit);
            if (exit) return;
            break;
        case INPUT_BACK:
            service_menu_back(menu, &app->mode, &app->display, &exit);
            if (exit) return;
            break;
        default: break;
    }

    service_menu_render(menu, &app->display);
}

static void service_menu_draw_item(ServiceMenu *menu, const DisplayContext *display, const MenuItem *item, bool selected, int line)
{
    switch (item->field)
    {
        case MENU_FIELD_IP:

            service_menu_draw_option(
                display,
                selected,
                line,
                "%s: %s",
                item->caption,
                menu->temporary_config.server_ip);

            break;

        case MENU_FIELD_PORT:

            service_menu_draw_option(
                display,
                selected,
                line,
                "%s: %d",
                item->caption,
                menu->temporary_config.server_port);

            break;
        case MENU_FIELD_PADDING:

            service_menu_draw_option(
                display,
                selected,
                line,
                "%s: %d",
                item->caption,
                menu->temporary_config.display_padding);

            break;
        case MENU_FIELD_FULLSCREEN:

            service_menu_draw_option(
                display,
                selected,
                line,
                "%s: %s",
                item->caption,
                menu->temporary_config.display_fullscreen ? "TAK" : "NIE");

            break;
        case MENU_FIELD_BACKGROUND:

            service_menu_draw_option(
                display,
                selected,
                line,
                "%s: %3d, %3d, %3d",
                item->caption,
                menu->temporary_config.background_color.r,
                menu->temporary_config.background_color.g,
                menu->temporary_config.background_color.b);

            break;
        case MENU_FIELD_FOREGROUND:

            service_menu_draw_option(
                display,
                selected,
                line,
                "%s: %3d, %3d, %3d",
                item->caption,
                menu->temporary_config.foreground_color.r,
                menu->temporary_config.foreground_color.g,
                menu->temporary_config.foreground_color.b);

            break;
    }
}

static void service_menu_overlay_render(const DisplayContext *display, bool hide)
{
    if (hide) return;

    display_draw_line_on_back_buffer(display, 0, "        MENU SERWISOWE");
    display_draw_line_on_back_buffer(display, 1, "-------------------------------");

    display_draw_line_on_back_buffer(display, 10, "-------------------------------");
}

static void service_menu_main_page_render(const ServiceMenu *menu, const DisplayContext *display)
{
    service_menu_draw_option(display, menu->selected_item == 0, 2, "SIEĆ");
    service_menu_draw_option(display, menu->selected_item == 1, 4, "EKRAN");

    service_menu_draw_option(display, menu->selected_item == 2, 7, "LICENCJE");
    service_menu_draw_option(display, menu->selected_item == 3, 9, "ZAPISZ / WYJDŹ");
}

static void service_menu_network_page_render(ServiceMenu *menu, const DisplayContext *display)
{
    for (int i = 0; i < SDL_arraysize(network_items); i++)
    {
        service_menu_draw_item(menu, display, &network_items[i], menu->selected_item == i, 2*i + 2);
    }
}

static void service_menu_display_page_render(ServiceMenu *menu, const DisplayContext *display)
{
    for (int i = 0; i < SDL_arraysize(display_items); i++)
    {
        service_menu_draw_item(menu, display, &display_items[i], menu->selected_item == i, 2*i + 2);
    }
}

static void service_menu_license_page_render(ServiceMenu *menu, const DisplayContext *display)
{
    display_draw_line_on_back_buffer(display, 0, APP_NAME " v" APP_VERSION_STRING);
    display_draw_line_on_back_buffer(display, 1, "Copyright (c) 2026 " APP_COMPANY);
    display_draw_line_on_back_buffer(display, 2, "Licensed under GNU GPL-2.0-only");

    display_draw_line_on_back_buffer(display, 3, "Uses SDL3, SDL3_ttf, SDL3_net (zlib),");
    display_draw_line_on_back_buffer(display, 4, "inih (BSD-3-Clause), stb_image (MIT)");

    display_draw_line_on_back_buffer(display, 5, "Fonts:");
    display_draw_line_on_back_buffer(display, 6, "CozetteVector (Samhain & contrib, MIT)");
    display_draw_line_on_back_buffer(display, 7, "MiniForma2, MiniSet2 (Bartek Nowak)");
    display_draw_line_on_back_buffer(display, 8, "Monocraft (Idress Hassan, SIL OFL 1.1)");
    display_draw_line_on_back_buffer(display, 9, "FreeSans (GNU Project, GNU GPL v3)");
    display_draw_line_on_back_buffer(display, 10, "Third-party licenses included.");
    display_draw_line_on_back_buffer(display, 11, "Source code available with this software.");
}

void service_menu_render(ServiceMenu *menu, DisplayContext *display)
{
    display_clear(display);
    display_change_font(display, 4);
    service_menu_overlay_render(display, menu->hideOverlay);

    switch (menu->page)
    {
        case MENU_PAGE_MAIN:
            service_menu_main_page_render(menu, display);
            break;

        case MENU_PAGE_NETWORK:
            service_menu_network_page_render(menu, display);
            break;

        case MENU_PAGE_DISPLAY:
            service_menu_display_page_render(menu, display);
            break;

        case MENU_PAGE_LICENSE:
            service_menu_license_page_render(menu, display);
            break;
    }

    if (menu->edit_mode != EDIT_NONE)
    {
        service_menu_draw_option(display, true, 11, "%s", menu->input_buffer);
    }

    display_swap_buffers(display);
}