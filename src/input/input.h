// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#ifndef TABLICA_INPUT_H
#define TABLICA_INPUT_H

#include <SDL3/SDL.h>

typedef enum
{
    INPUT_NONE,

    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,

    INPUT_DIGIT,

    INPUT_OK,
    INPUT_BACK
} InputAction;

typedef struct
{
    InputAction action;

    int digit;
} InputEvent;

typedef struct
{
    const SDL_Keycode *sequence;

    Uint64 last_key_time;
    int length;
    int position;
} KeySequence;

void input_key_sequence_init(KeySequence *seq, const SDL_Keycode *keys, int length);
bool input_key_sequence_check(KeySequence *seq, SDL_Keycode key);

InputEvent input_translate(SDL_Keycode key);

#endif //TABLICA_INPUT_H
