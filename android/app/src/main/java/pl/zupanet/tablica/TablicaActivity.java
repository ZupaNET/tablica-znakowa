package pl.zupanet.tablica;

import org.libsdl.app.SDLActivity;

public class TablicaActivity extends SDLActivity {

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL3",
            "main"
        };
    }
}