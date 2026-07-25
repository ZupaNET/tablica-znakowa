// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#include "common/constants.h"
#include "display/display.h"
#include "server.h"
#include "state/app_state.h"

bool server_init(const Config *cfg, ServerContext *server)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Ładowanie podsystemu sieciowego");
    if (!NET_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można załadować SDL_net: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetAtomicInt(&server->server_running, 1);

    NET_Address *addr = NET_ResolveHostname(cfg->server_ip);
    if (!addr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można sparsować adresu IP: %s\n", SDL_GetError());
        return false;
    }

    NET_Status status = NET_WaitUntilResolved(addr, 5000);
    if (status != NET_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można rozwiązać adresu IP: %s\n", SDL_GetError());
        NET_UnrefAddress(addr);
        return false;
    }

    server->server = NET_CreateServer(addr, cfg->server_port, 0);
    if (!server->server)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Nie można utworzyć serwera: %s\n", SDL_GetError());
        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: Uruchomiono serwer %s:%d", cfg->server_ip, cfg->server_port);
    return true;
}

void server_parse_client_command(DisplayContext *display, Client *client, const char *line)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] Klient %s wysłał wiadomość: %s", client->ip, line);

    if (*line == '\0')
        return;

    NET_WriteToStreamSocket(client->socket, "ok\n", 3);

    if (SDL_strncmp(line, "qq0", 3) == 0)
    {
        SDL_Event event = {0};
        event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&event);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] Zażądano zamknięcia tablicy!", client->ip);
    }
    else if (SDL_strncmp(line, "l", 1) == 0)
    {
        if (SDL_strlen(line) < 4)
            return;

        if (!SDL_isdigit(line[1]) || !SDL_isdigit(line[2]))
            return;

        const int line_number = (line[1] - '0') * 10 + (line[2] - '0');
        const char *line_begin = line + 3;

        display_draw_line_on_back_buffer(display, line_number, line_begin);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] Załadowano linię %d: %s", client->ip, line_number, line_begin);
    }
    else if (SDL_strncmp(line, "f", 1) == 0)
    {
        if (SDL_strlen(line) < 3)
            return;

        const int font_number = (line[1] - '0') * 10 + (line[2] - '0');
        if (font_number > DISPLAY_MAX_FONTS)
            return;

        display_change_font(display, font_number);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] Zmieniono czcionkę: %d", client->ip, font_number);
    }
    else if (SDL_strncmp(line, "go0", 3) == 0)
    {
        display_swap_buffers(display);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] Wyświetlono bufor na tablicy", client->ip);
    }
    else if (SDL_strncmp(line, "j", 1) == 0)
    {
        if (SDL_strlen(line) < 3)
            return;

        const unsigned int brightness = (line[1] - '0') * 10 + (line[2] - '0');
        if (brightness > 4)
            return;

        display->back_buffer->brightness = brightness;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] Ustawiono nową jasność: %d", client->ip, brightness);
    }
    else if (SDL_strncmp(line, "wy0", 3) == 0)
    {
        display_commit_brightness(display);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "INFO: [%s] Zatwierdzono nową jasność", client->ip);
    }

}

void server_destroy(ServerContext *server)
{
    if (!server)
        return;

    if (!server->server)
        return;

    SDL_SetAtomicInt(&server->server_running, 0);
    SDL_WaitThread(server->server_thread, nullptr);
    NET_DestroyServer(server->server);
    NET_Quit();
}

int server_thread(void *userdata)
{
    AppState *state = (AppState *)userdata;
    ServerContext *server = &state->server;
    DisplayContext *display = &state->display;

    Client clients[SERVER_MAXIMUM_CLIENTS] = {0};

    while (SDL_GetAtomicInt(&server->server_running))
    {
        NET_StreamSocket *sock = nullptr;

        while (NET_AcceptClient(server->server, &sock) && sock)
        {
            int slot = -1;

            for (int i = 0; i < SERVER_MAXIMUM_CLIENTS; i++)
            {
                if (!clients[i].socket)
                {
                    slot = i;
                    break;
                }
            }

            if (slot < 0)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[WARN] Wyczerpano pulę dostępnych slotów dla klientów!");
                NET_DestroyStreamSocket(sock);
                continue;
            }

            clients[slot].socket = sock;
            clients[slot].used = 0;

            NET_Address *addr = NET_GetStreamSocketAddress(sock);

            if (addr)
            {
                const char *ip = NET_GetAddressString(addr);

                if (ip)
                    SDL_strlcpy(clients[slot].ip, ip, sizeof(clients[slot].ip));

                NET_UnrefAddress(addr);
            }

            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] Połączono z klientem: %s", clients[slot].ip);
        }

        for (int i = 0; i < SERVER_MAXIMUM_CLIENTS; i++)
        {
            Client* client = &clients[i];

            if (!client->socket)
                continue;

            char temp[SERVER_BUFFER_SIZE];

            int received = NET_ReadFromStreamSocket(client->socket, temp, sizeof(temp));

            if (received == 0)
                continue;

            if (received < 0)
            {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] Rozłączono klienta: %s", client->ip);
                NET_DestroyStreamSocket(client->socket);

                client->socket = nullptr;
                client->used = 0;

                continue;
            }

            if (client->used + received > SERVER_BUFFER_SIZE)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[WARN] Nastąpiło przepełnienie bufora u klienta %s!", client->ip);
                NET_DestroyStreamSocket(client->socket);

                client->socket = nullptr;
                client->used = 0;

                continue;
            }

            SDL_memcpy(client->buffer + client->used, temp, received);
            client->used += received;

            char *begin = client->buffer;

            while (1)
            {
                size_t available = client->used - (begin - client->buffer);

                char *newline = memchr(begin, '\n', available);

                if (!newline)
                    break;

                *newline = '\0';
                if (newline > begin && newline[-1] == '\r')
                    newline[-1] = '\0';

                // Parse only when in display mode
                if (state->mode == APP_MODE_DISPLAY)
                    server_parse_client_command(display, client, begin);

                begin = newline + 1;
            }

            size_t remaining = client->used - (begin - client->buffer);

            SDL_memmove(client->buffer, begin, remaining);

            client->used = remaining;
        }

        SDL_Delay(1);
    }

    for (int i = 0; i < SERVER_MAXIMUM_CLIENTS; i++)
    {
        if (clients[i].socket)
            NET_DestroyStreamSocket(clients[i].socket);
    }

    return 0;
}