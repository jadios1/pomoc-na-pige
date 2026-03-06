#define NOMINMAX
#include <windows.h>
#include "app_2048.h"

// Punkt wejścia programu — zamiast main() używamy wWinMain()
// hInstance   — "bilet" do naszego programu (daje go Windows)
// show_command — jak pokazać główne okno (normalnie, zmaksymalizowane, itd.)
int WINAPI wWinMain(HINSTANCE hInstance,
    HINSTANCE /*prevInstance*/,   // zawsze nullptr, ignorujemy
    LPWSTR    /*command_line*/,   // argumenty z linii poleceń, ignorujemy
    int       show_command)
{
    // Tworzymy obiekt aplikacji — w konstruktorze rejestruje klasę i tworzy okna
    app_2048 app{ hInstance };

    // Uruchamiamy pętlę wiadomości — program "żyje" tutaj aż do zamknięcia
    return app.run(show_command);
}
