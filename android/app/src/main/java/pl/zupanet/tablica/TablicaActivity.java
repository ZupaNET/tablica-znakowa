// SPDX-License-Identifier: GPL-2.0-only
/*
 * Tablica Znakowa
 *
 * Copyright (C) 2026 ŻupaNET Development <devel@zupanet.pl>
 */

package pl.zupanet.tablica;

import org.libsdl.app.SDLActivity;

public class TablicaActivity extends SDLActivity {

    @Override
    protected String[] getLibraries() {
        return new String[] {
			"c++_shared",
            "SDL3",
            "SDL3_net",
            "SDL3_ttf",
            "tablica"
        };
    }
}