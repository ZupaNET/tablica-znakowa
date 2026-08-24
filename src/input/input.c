// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

#include "input.h"

void input_key_sequence_init(KeySequence *seq, const SDL_Keycode *keys, int length)
{
    seq->sequence = keys;
    seq->length = length;
    seq->position = 0;
}

bool input_key_sequence_check(KeySequence *seq, SDL_Keycode key)
{
    if (SDL_GetTicks() - seq->last_key_time > 5000)
    {
        seq->position = 0;
    }

    if (key == seq->sequence[seq->position])
    {
        seq->position++;
        seq->last_key_time = SDL_GetTicks();

        if (seq->position >= seq->length)
        {
            seq->position = 0;
            return true;
        }

        return false;
    }

    if (key == seq->sequence[0])
        seq->position = 1;
    else
        seq->position = 0;

    return false;
}

InputEvent input_translate(SDL_Keycode key)
{
    InputEvent ev; ev.digit = 0;

    switch (key)
    {
        case SDLK_UP: ev.action = INPUT_UP; break;
        case SDLK_DOWN: ev.action = INPUT_DOWN; break;
        case SDLK_LEFT: ev.action = INPUT_LEFT; break;
        case SDLK_RIGHT: ev.action = INPUT_RIGHT; break;
        case SDLK_RETURN: ev.action = INPUT_OK; break;
        case SDLK_AC_BACK:
        case SDLK_ESCAPE: ev.action = INPUT_BACK; break;

        case SDLK_0: ev.action = INPUT_DIGIT; ev.digit = 0; break;
        case SDLK_1: ev.action = INPUT_DIGIT; ev.digit = 1; break;
        case SDLK_2: ev.action = INPUT_DIGIT; ev.digit = 2; break;
        case SDLK_3: ev.action = INPUT_DIGIT; ev.digit = 3; break;
        case SDLK_4: ev.action = INPUT_DIGIT; ev.digit = 4; break;
        case SDLK_5: ev.action = INPUT_DIGIT; ev.digit = 5; break;
        case SDLK_6: ev.action = INPUT_DIGIT; ev.digit = 6; break;
        case SDLK_7: ev.action = INPUT_DIGIT; ev.digit = 7; break;
        case SDLK_8: ev.action = INPUT_DIGIT; ev.digit = 8; break;
        case SDLK_9: ev.action = INPUT_DIGIT; ev.digit = 9; break;

        case SDLK_PERIOD:
        case SDLK_COMMA: ev.action = INPUT_DELIMITER; break;
        default: ev.action = INPUT_NONE; break;
    }

    return ev;
}