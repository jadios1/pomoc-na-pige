#pragma once
// #pragma once — ten plik zostanie wczytany tylko raz przez kompilator
// (zabezpieczenie przed podwójnym includowaniem)

#define NOMINMAX
#include <windows.h>
#include <string>

class app_2048
{
public:
    // Konstruktor — tworzy aplikację, rejestruje klasę okna, tworzy oba okna
    // Wywołujesz: app_2048 app{ hInstance };
    app_2048(HINSTANCE instance);

    // Uruchamia pętlę wiadomości — program tu "żyje" aż użytkownik zamknie okno
    // Zwraca 0 gdy wszystko ok, lub kod błędu
    int run(int show_command);

private:
    // --- DANE (pola klasy) ---

    HINSTANCE m_instance;   // "bilet" do naszego programu
    HWND      m_main;       // uchwyt (handle) do głównego okna
    HWND      m_popup;      // uchwyt do drugiego okna (pomocniczego)

    // Nazwa klasy okna — static znaczy że jest jedna dla wszystkich obiektów klasy
    // (nie ma sensu tworzyć osobnej kopii dla każdego obiektu)
    static std::wstring const s_class_name;

    // --- METODY PRYWATNE ---

    // Rejestruje klasę okna u Windowsa — opisuje jak okno ma wyglądać
    bool register_class();

    // Tworzy okno o podanych stylach i rodzicu
    // style    — jakie przyciski/ramki ma mieć okno
    // parent   — okno nadrzędne (nullptr = brak, czyli okno główne)
    HWND create_window(DWORD style, HWND parent = nullptr);

    // --- OBSŁUGA WIADOMOŚCI ---

    // Statyczna WndProc — Windows MUSI móc ją wywołać bez obiektu
    // (funkcje składowe klasy nie mogą być bezpośrednio przekazane do WinAPI)
    // Ona wyciąga wskaźnik na obiekt i przekazuje do window_proc()
    static LRESULT CALLBACK window_proc_static(
        HWND window, UINT message, WPARAM wparam, LPARAM lparam);

    // Właściwa obsługa wiadomości — tu piszemy co ma się dziać
    // przy zamknięciu, klawiszach, rysowaniu, itd.
    LRESULT window_proc(
        HWND window, UINT message, WPARAM wparam, LPARAM lparam);
};
