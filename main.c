#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_net/SDL_net.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define SERVER_PORT 60023
#define MAX_CLIENTS 64
#define BUFFER_SIZE 8192
#define MAX_LINES 12
#define PADDING 8

struct DisplayState
{
    char* lines[MAX_LINES];
    unsigned brightness; // From 00 to 04
    TTF_Font *font; // From 00 to 02
};

struct Client
{
    NET_StreamSocket *socket;
    char buffer[BUFFER_SIZE];
    size_t used;
    char ip[64];
};

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

static TTF_Font *miniset2_font = nullptr;
static TTF_Font *miniforma2_font = nullptr;
static TTF_Font *sans_serif_font = nullptr;

static NET_Server *server = nullptr;
static SDL_Thread *server_thread = nullptr;
static SDL_AtomicInt server_running;

static struct DisplayState buffer1, buffer2;
static struct DisplayState* back_buffer = &buffer1;
static struct DisplayState* front_buffer = &buffer2;
static SDL_Mutex *buffer_mutex = nullptr;

extern const unsigned char MiniSet2_ttf[];
extern const unsigned int MiniSet2_ttf_len;
extern const unsigned char MiniForma2_ttf[];
extern const unsigned int MiniForma2_ttf_len;
extern const unsigned char sans_serif_ttf[];
extern const unsigned int sans_serif_ttf_len;

#ifdef _WIN32
static void DetachConsoleIfOwned()
{
    HWND hwnd = GetConsoleWindow();

    if (!hwnd)
        return;

    DWORD processes[16];
    DWORD count = GetConsoleProcessList(
        processes,
        SDL_arraysize(processes));

    if (count == 1)
    {
        FreeConsole();
    }
}
#endif

static int GetLineSpacing(const TTF_Font *font)
{
    if (font == miniset2_font) return 10;
    if (font == miniforma2_font) return 12;
    if (font == sans_serif_font) return 14;
    return 10;
}

static void SwapDisplayBuffers()
{
    SDL_LockMutex(buffer_mutex);

    struct DisplayState* tmp = front_buffer;
    front_buffer = back_buffer;
    back_buffer = tmp;

    SDL_UnlockMutex(buffer_mutex);
}

static void ParseCommand(struct Client *client, const char *line)
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

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] [%s] Zażądano zamknięcia tablicy!", client->ip);
    }
    else if (SDL_strncmp(line, "l", 1) == 0)
    {
        if (SDL_strlen(line) < 4)
            return;

        if (!SDL_isdigit(line[1]) || !SDL_isdigit(line[2]))
            return;

        const unsigned int line_number = (line[1] - '0') * 10 + (line[2] - '0');
        if (line_number >= MAX_LINES)
            return;

        const char *line_begin = line + 3;
        const size_t line_length = SDL_strlen(line_begin);

        char *new_line = SDL_realloc(back_buffer->lines[line_number],line_length + 1);
        if (!new_line)
            return;
        back_buffer->lines[line_number] = new_line;

        SDL_strlcpy(back_buffer->lines[line_number], line_begin, line_length + 1);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] [%s] Załadowano linię %d: %s", client->ip, line_number, back_buffer->lines[line_number]);
    }
    else if (SDL_strncmp(line, "f", 1) == 0)
    {
        if (SDL_strlen(line) < 3)
            return;

        const unsigned int font_number = (line[1] - '0') * 10 + (line[2] - '0');
        if (font_number > 2)
            return;

        switch (font_number)
        {
            case 0:
                back_buffer->font = miniset2_font;
                break;
            case 1:
                back_buffer->font = miniforma2_font;
                break;
            case 2:
                back_buffer->font = sans_serif_font;
                break;
            default:
                break;
        }

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] [%s] Zmieniono czcionkę: %d", client->ip, font_number);
    }
    else if (SDL_strncmp(line, "go0", 3) == 0)
    {
        SwapDisplayBuffers();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] [%s] Wyświetlono bufor na tablicy", client->ip);
    }
    else if (SDL_strncmp(line, "j", 1) == 0)
    {
        if (SDL_strlen(line) < 3)
            return;

        const unsigned int brightness = (line[1] - '0') * 10 + (line[2] - '0');
        if (brightness > 4)
            return;

        back_buffer->brightness = brightness;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] [%s] Ustawiono nową jasność: %d", client->ip, brightness);
    }
    else if (SDL_strncmp(line, "wy0", 3) == 0)
    {
        SDL_LockMutex(buffer_mutex);
        front_buffer->brightness = back_buffer->brightness;
        SDL_UnlockMutex(buffer_mutex);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] [%s] Zatwierdzono nową jasność", client->ip);
    }

}

static int ServerThread(void *userdata)
{

    struct Client clients[MAX_CLIENTS] = {0};

    while (SDL_GetAtomicInt(&server_running))
    {
        NET_StreamSocket *sock = nullptr;

        while (NET_AcceptClient(server, &sock) && sock)
        {
            int slot = -1;

            for (int i = 0; i < MAX_CLIENTS; i++)
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

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            struct Client* client = &clients[i];

            if (!client->socket)
                continue;

            char temp[1024];

            int received = NET_ReadFromStreamSocket(client->socket, temp, sizeof(temp));

            if (received == 0)
                continue;

            if (received < 0)
            {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] Rozłączono klienta: %s", client->ip);
                NET_DestroyStreamSocket((client->socket));

                client->socket = nullptr;
                client->used = 0;

                continue;
            }

            if (client->used + received > BUFFER_SIZE)
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[WARN] Nastąpiło przepełnienie bufora u klienta %s!", client->ip);
                NET_DestroyStreamSocket(client->socket);

                client->socket = nullptr;
                client->used = 0;

                continue;
            }

            memcpy(client->buffer + client->used, temp, received);
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

                ParseCommand(client, begin);

                begin = newline + 1;
            }

            size_t remaining = client->used - (begin - client->buffer);

            memmove(client->buffer, begin, remaining);

            client->used = remaining;
        }

        SDL_Delay(1);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket)
            NET_DestroyStreamSocket(clients[i].socket);
    }

    return 0;
}

static bool CreateServer()
{
    SDL_SetAtomicInt(&server_running, 1);

    server = NET_CreateServer(nullptr, SERVER_PORT, 0);
    if (!server)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można utworzyć serwera: %s\n", SDL_GetError());
        return false;
    }

    server_thread = SDL_CreateThread(ServerThread, "Server", nullptr);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Uruchomiono serwer na porcie %d", SERVER_PORT);
    return true;
}

static bool LoadFonts()
{
    miniset2_font = TTF_OpenFontIO(SDL_IOFromConstMem(MiniSet2_ttf, MiniSet2_ttf_len), true, 64.0f);
    if (!miniset2_font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można załadować czcionki MiniSet2: %s\n", SDL_GetError());
        return false;
    }

    miniforma2_font = TTF_OpenFontIO(SDL_IOFromConstMem(MiniForma2_ttf, MiniForma2_ttf_len), true, 64.0f);
    if (!miniforma2_font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można załadować czcionki MiniForma2: %s\n", SDL_GetError());
        return false;
    }

    sans_serif_font = TTF_OpenFontIO(SDL_IOFromConstMem(sans_serif_ttf, sans_serif_ttf_len), true, 64.0f);
    if (!sans_serif_font)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można załadować czcionki Sans Serif: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

static void InitDisplayBuffers()
{
    buffer1.brightness = 4;
    buffer1.font = miniset2_font;
    for (int i = 1; i < MAX_LINES; i++)
    {
        buffer1.lines[i] = (char*)SDL_malloc(1);
        buffer1.lines[i][0] = '\0';
    }
    buffer1.lines[0] = (char*)SDL_malloc(2);
    buffer1.lines[0][0] = '.'; buffer1.lines[0][1] = '\0';

    buffer2.brightness = 4;
    buffer2.font = miniset2_font;
    for (int i = 1; i < MAX_LINES; i++)
    {
        buffer2.lines[i] = (char*)SDL_malloc(1);
        buffer2.lines[i][0] = '\0';
    }
    buffer2.lines[0] = (char*)SDL_malloc(2);
    buffer2.lines[0][0] = '.'; buffer2.lines[0][1] = '\0';
}

void PrintHeader()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"\n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"          ┏┳┓╻┏ ┏━╸╻┏━┓      \n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"       ┃┏┛┃┃┃┣┻┓┣╸ ┃┣━┫      \n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"       ┗┛ ╹ ╹╹ ╹┗━╸╹╹ ╹      \n");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,"\n");
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
#ifdef _WIN32
    DetachConsoleIfOwned();
#endif

    PrintHeader();

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Ładowanie aplikacji vMKEiA");
    if (!SDL_CreateWindowAndRenderer("vMKEiA - Wirtualna tablica LED", 1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można utworzyć okna i renderera: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Ładowanie podsystemu sieciowego");
    if (!NET_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można załadować SDL_net: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Ładowanie podsystemu czcionek");
    if (!TTF_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"[ERROR] Nie można załadować SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Ładowanie czcionek");
    if (!LoadFonts())
        return SDL_APP_FAILURE;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Inicjalizacja buforów");
    InitDisplayBuffers();
    buffer_mutex = SDL_CreateMutex();

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Ładowanie serwera");
    if (!CreateServer())
        return SDL_APP_FAILURE;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Aplikacja vMKEiA jest gotowa do działania!");
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    /* Clear screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Get buffer pointer copy */
    SDL_LockMutex(buffer_mutex);
    struct DisplayState *state = front_buffer;
    SDL_UnlockMutex(buffer_mutex);

    if (!state || !state -> font)
    {
        SDL_RenderPresent(renderer);
        return SDL_APP_CONTINUE;
    }

    int screen_w, screen_h;
    SDL_GetRenderOutputSize(renderer, &screen_w, &screen_h);

    const float factor = (float)state->brightness / 4.0f;
    const SDL_Color base = {255, 69, 0, 255};
    const SDL_Color color =
    {
        .r = (Uint8)((float)base.r * factor),
        .g = (Uint8)((float)base.g * factor),
        .b = (Uint8)((float)base.b * factor),
        .a = 255
    };

    int font_height = TTF_GetFontHeight(state->font);
    int spacing = GetLineSpacing(state->font);

    float line_height = (float)(font_height + spacing);

    float available_height = (float)screen_h - (PADDING * 2);

    float scale = available_height / (line_height * MAX_LINES);

    int x = PADDING;
    float y = PADDING;

    for (int i = 0; i < MAX_LINES; i++)
    {

        const char *text = state->lines[i];
        if (!text || !text[0])
            continue;

        SDL_Surface *surface = TTF_RenderText_Blended(state->font, text, 0, color);
        if (!surface)
            continue;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_DestroySurface(surface);
            continue;
        }

        SDL_FRect dst = {
            .x = (float)x,
            .y = y,
            .w = (float)surface->w * scale,
            .h = (float)surface->h * scale
        };

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);

        SDL_RenderTexture(renderer, texture, nullptr, &dst);

        y += line_height * scale;

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[INFO] Zamykam aplikację!");

    if (miniset2_font)
        TTF_CloseFont(miniset2_font);

    if (miniforma2_font)
        TTF_CloseFont(miniforma2_font);

    if (sans_serif_font)
        TTF_CloseFont(sans_serif_font);

    if (server)
    {
        SDL_SetAtomicInt(&server_running, 0);
        SDL_WaitThread(server_thread, nullptr);
        NET_DestroyServer(server);
    }

    SDL_DestroyMutex(buffer_mutex);

    TTF_Quit();
    NET_Quit();
}