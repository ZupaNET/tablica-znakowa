<p align="center"><img src="./fastlane/metadata/android/pl-PL/images/icon.png" height="200" style="border-radius: 50%" /></p>
<h1 align="center"> Tablica Znakowa</h1>
<p align="center">Tablica Znakowa to emulator klasycznej znakowej tablicy LED od firmy MKEiA.</p>

<p align="center">
  <img src="https://img.shields.io/badge/Made_in-Żupawa-000000?style=for-the-badge&"/>
  <img src="https://img.shields.io/badge/Coded_by-ŻupaNET-000000?style=for-the-badge&"/>
  <img src="https://img.shields.io/badge/Written_in_C-ONLY!-000000?style=for-the-badge&logo=C"/>
</p>

## Przegląd

Tablica Znakowa to nieskomplikowany, lekki i otwarto-źródłowy emulator tablicy znakowej LED produkowanej dawniej 
przez firmę MKEiA do wyświetlania pieśni kościelnych.

- Współpraca z aplikacjami MKEiA: Edytor Pieśni i mobilne Pieśni!
- Wbudowane czcionki używane na prawdziwej tablicy!
- Menu serwisowe do prostej konfiguracji tablicy z poziomu klawiatury/pilota!
- Obsługa najpopularniejszych platform dla tablic wyświetlających: Windows, Linux i Android TV!
- Niewielki rozmiar aplikacji i mała liczba zależności!

**Uwaga!** Tablica Znakowa to jedynie emulator! Nie posiada żadnej pamięci i nie pozwala wyświetlać nic sam z siebie.
Do pełnego działania wymagana jest również aplikacja kliencka: *Pieśni* lub *Edytor Pieśni* od MKEiA albo [*Prezenter*](http://192.168.40.2:3000/ZupaNET-publiczne/tablica) od
ŻupaNET Development.

<p align=center>
  <img height="128" alt="2" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/02.png" />
  <img height="128" alt="3" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/03.png" />
  <img height="128" alt="4" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/04.png" />
  <img height="128" alt="5" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/05.png" />
  <img height="128" alt="6" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/06.png" />
  <img height="128" alt="7" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/07.png" />
  <img height="128" alt="8" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/08.png" />
  <img height="128" alt="9" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/09.png" />
  <img height="128" alt="10" src="./fastlane/metadata/android/pl-PL/images/tvScreenshots/10.png" />
</p>

## Instalacja

Aplikacja nie wymaga dodatkowych zależności do działania poza tym, co jest dostarczane razem z nią. 
Dokładne opisy instalacji w zależności od platformy dostępne są poniżej.

#### Windows, Android, Debian-alike, AlmaLinux-alike

Należy pobrać instalator/paczkę z zakładki **Wydania** odpowiednią dla swojej architektury i zainstalować/wypakować. 
W przypadku systemów Debian (i pochodnych) oraz Alma Linux (i pochodnych) aplikację zbudowano na systemie Debian 13
oraz AlmaLinux OS 10 - trzeba to wziąć pod uwagę przy instalacji paczek DEB/RPM na systemach pochodnych.

> **UWAGA!** Biblioteka SDL3_net na dzień 27.07.2026 jest w repozytoriach Debiana w fazie testów. Może zajść konieczność
> dodania repozytorium testowego lub zainstalowania bezpośrednio paczki deb biblioteki SDL3_net. Więcej informacji [tutaj](https://wiki.debian.org/DebianExperimental).
> Ewentualnie dostępna jest paczka deb z dołączonym SDL3 - ostrożnie jednak z instalacją na systemach ogólnego przeznaczenia.

### Pozostałe systemy Linux

Ze względu na charakterystykę aplikacji zalecane jest skompilowanie jej na docelowej platformie. Patrz [**Kompilacja**](#kompilacja)

## Konfiguracja

Tablica Znakowa nie wymaga dodatkowej konfiguracji. Po uruchomieniu jest gotowa do działania. Domyślnie nasłuchuje na
wszystkich adresach IP i na porcie **60023**. Na Windows i Linux istnieje opcja zmiana niektórych ustawień w pliku konfiguracyjnym:
- Windows: `%appdata%\ŻupaNET Development\Tablica Znakowa\tablica.ini`
- Linux: `~/.local/config/ŻupaNET Development/Tablica Znakowa/tablica.ini`

W przypadku Androida nie ma możliwości łatwej modyfikacji pliku konfiguracyjnego zatem należy skorzystać z [**Menu Serwisowego**](#menu-serwisowe)

Opis pliku konfiguracyjnego:
- `ip` - adres IP, na którym tablica nasłuchuje
- `port` - port, na którym tablica nasłuchuje
- `padding` - odstęp (w pikselach) tekstu od lewej i górnej krawędzi ekranu
- `fullscreen` - włącza tryb pełnoekranowy (na Androidzie jest zawsze włączony)
- `background` - kolor tła w formacie hex (np. #000000)
- `foreground` - kolor czcionki w formacie hex (np. #FF0000)

Przykładowy plik konfiguracyjny:
```ini
[server]
ip=0.0.0.0
port=60023

[display]
padding=8
fullscreen=0
background=#000000
foreground=#ff0000
```

# Menu Serwisowe

Dla systemów opartych o Android (głównie telewizorów) i tam, gdzie nie ma możliwości modyfikacji plików systemowych, zostało
przygotowane menu serwisowe, które pozwala zmieniać wyżej wskazane ustawienia. Aby przejść do menu serwisowego należy po 
uruchomieniu tablicy wcisnąć w ciągu 5 sekund kombinację: **GÓRA GÓRA DÓŁ DÓŁ LEWO PRAWO LEWO PRAWO OK/ENTER**. Menu można
opuścić bez zapisywania naciskając **ESC/BACK**. D-Padem/strzałkami możemy poruszać się po menu. Kliknięcie OK/ENTER wybiera
opcję, z kolei ESC/BACK anuluje wybór. 

Po wybraniu danej opcji do edycji możemy klawiaturą numeryczną wpisywać cyfry. Przyciskiem OK/ENTER zatwierdzamy, a ESC/BACK
kasujemy ostatnią cyfrę/znak. Jeżeli skasujemy wszystkie cyfry/znaki to ponowne naciśnięcie ESC/BACK anuluje edycję opcji.

**Jeżeli wpisujemy adres IP lub kolor to naciśnięcie przycisku RIGHT wpisze nam kolejno kropkę lub przecinek.**

Po zapisaniu ustawień tablica powróci do trybu wyświetlania, ale nowe ustawienia zostaną zapisane dopiero po zrestartowaniu
aplikacji.

**Wyłączyć aplikację możemy również w trybie wyświetlania naciskająć ESC/BACK**.

# Kompilacja

Kompilacja aplikacji na wszystkie platformy jest bardzo prosta, ale na początku należy zainstalować potrzebne narzędzia.
Dla systemu Windows zalecane jest zainstalowanie kompilatora **LLVM+CLang+MinGW** (np. [tego](https://github.com/mstorsjo/llvm-mingw) - **najlepiej za pomocą WinGet**),
a także narzędzia **CMake** oraz dowolnego interpretera **Python** (wymagany do kompilacji SDL3). W przypadku systemu Linux
wykorzystujemy wybrany przez nas kompilator (własny albo z repozytorium) oraz jeżeli chcemy - biblioteki SDL3 z repozytorium dystrybucji.
Dla Androida to, co Google Inc. dla nas przygotował. Tablica Znakowa zadziała na wszystkim, na czym zadziała SDL3, SDL3_ttf i SDL3_net. Do Windowsa przyda się również InnoSetup,
jeżeli interesuje nas generowanie instalatorów.

### Instalacja zależności

Jeżeli kompilujemy SDL3 na X11/Wayland ze źródeł w ramach projektu musimy również doinstalować zależności potrzebne podane na [wiki libSDL](https://wiki.libsdl.org/SDL3/README-linux#build-dependencies).  
Jeżeli nie interesuje nas X11/Wayland to możemy dodać opcję `-DSDL_UNIX_CONSOLE_BUILD=ON` do komendy `cmake` z punktu 2.

#### Debian / Ubuntu / Linux Mint i pochodne
```sh
sudo apt install build-essential git make \
pkg-config cmake ninja-build libsdl3-dev \
libsdl3-ttf-dev libsdl3-net-dev
```

#### Alma Linux OS / Rocky Linux / RHEL i pochodne
```sh
sudo dnf install git gcc gcc-c++ cmake \
    ninja-build pkgconf-pkg-config rpmdevtools
```
Jeżeli nasza dystrybucja na to pozwala, to można spróbować doinstalować SDL3 z repozytoriów RPM: `sudo dnf install SDL3-devel SDL3_ttf-devel SDL3_net-devel`.
Niestety na dzień 27.07.2026, RHEL i pochodne w ogóle nie udostępniają SDL3, a Fedora już tak.


### Właściwy proces - Windows i Linux
1. Klonujemy repozytorium: `git clone http://192.168.40.2:3000/ZupaNET-publiczne/vmkeia.git`
2. Wywołujemy odpowiedni preset CMake z katalogu głównego projektu: `cmake --preset <nazwa>` (listę można wyświetlić: `cmake --list-presets`). Dodając ewentualnie `-DSDL_UNIX_CONSOLE_BUILD=ON`
3. Kompilujemy projekt: `cmake --build --preset <nazwa>` (listę można wyświetlić: `cmake --build --list-presets`)

Jeżeli wszystko się uda, to aplikacja zostanie wybudowana w katalogu:
- Windows: `build/<preset>/bin`
- Linux: główny plik wykonywalny i zasoby `build/<preset>/bin`, a biblioteki `build/<preset>/lib`

Aby wybudować paczki DEB, RPM, TGZ (Linux) czy ZIP oraz instalator Inno Setup (Windows) to należy przejść do katalogu `build/<preset>/`,
a następnie wydać polecenie `cpack -G <paczka>`, gdzie `<paczka>` to może być **DEB**, **RPM**, **TGZ**, **ZIP**, **INNOSETUP**. 
Wybudowany instalator/paczka będzie w tym samym katalogu.

Na systemach Linux możemy również zainstalować od razu aplikację wykonując polecenie `cmake --install build/<preset>`. **Uwaga!**
Polecenie to **NIE** zainstaluje bibliotek SDL w `/usr/lib64/` jeżeli te zostały skompilowane w ramach projektu i nie użyto
tych dostarczanych z dystrybucją. Należy je ręcznie przekopiować do odpowiedniego katalogu.

### Właściwy proces - Android
1. Klonujemy repozytorium: `git clone http://192.168.40.2:3000/ZupaNET-publiczne/vmkeia.git`
2. Uruchamiamy terminal w repozytorium ze skonfigurowanym środowiskiem Android SDK, a także ustawionymi zmiennymi `ANDROID_KEYSTORE`, `ANDROID_KEYSTORE_PASSWORD`, `ANDROID_KEY_ALIAS` oraz `ANDROID_KEY_PASSWORD`
3. Przechodzimy do katalogu `android` i wydajemy polecenie: `gradlew assembleRelease` lub `gradlew assembleDebug` (dla wersji Debug).
4. W katalogu `app\build\outputs\apk\<debug/release>` zostanie wygenerowany plik APK na platformy `x86_64`, `arm64-v8a` oraz `armeabi-v7a`.

# Licencja

<p>Copyright (C) 2026 ŻupaNET Development

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

The above copyright notice, this permission notice, and its license shall be included in all copies or substantial portions of the Software.

You can find a copy of the GNU General Public License v3 [here](https://www.gnu.org/licenses/)</p>

## Biblioteki i zasoby osób trzecich

Informacje o licencjach bibliotek i zasobów osób trzecich wykorzystanych w projekcie można odnaleźć w pliku [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md)
