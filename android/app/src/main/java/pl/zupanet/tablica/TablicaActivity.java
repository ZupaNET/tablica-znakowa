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