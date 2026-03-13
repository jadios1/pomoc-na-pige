/ ═══════════════════════════════════════════════════════
// KOD 1 — samo okno, nic więcej
// ═══════════════════════════════════════════════════════
#include <windows.h>

// ── Deklaracja funkcji obsługi komunikatów ──────────────
// Musi być przed WinMain bo WinMain ją używa
// LRESULT — typ zwracany (liczba dla Windowsa)
// CALLBACK — konwencja wywołania wymagana przez Windows
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ── Punkt wejścia programu ──────────────────────────────
// WinMain zamiast main() — wymagane przez WinAPI
// hInst     = identyfikator tej aplikacji w systemie
// nCmdShow  = czy okno ma być zmaksymalizowane/normalne/etc
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {

    // ── Opis klasy okna ─────────────────────────────────
    // WNDCLASS = "przepis" na okno, wypełniasz pola i rejestrujesz
    WNDCLASS wc = {};                         // zeruj wszystkie pola
    wc.lpfnWndProc   = WndProc;              // wskaźnik na Twoją funkcję obsługi
    wc.hInstance     = hInst;                 // do której aplikacji należy klasa
    wc.lpszClassName = L"KlasaOkna";         // unikalna nazwa klasy (dowolna)
    wc.hbrBackground = CreateSolidBrush(RGB(30, 80, 120)); // kolor tła okna
    //              ↑ RGB(czerwony 0-255, zielony 0-255, niebieski 0-255)
    RegisterClass(&wc);                       // zarejestruj "przepis" w systemie

    // ── Tworzenie okna ──────────────────────────────────
    // CreateWindow zwraca HWND = uchwyt (identyfikator) okna
    HWND hwnd = CreateWindow(
        L"KlasaOkna",          // klasa (zarejestrowana wyżej)
        L"Moje pierwsze okno", // tekst w pasku tytułu

        // styl okna — flagi połączone przez | (bitowe OR)
        WS_OVERLAPPED  |        // podstawowe okno z ramką
        WS_CAPTION     |        // pasek tytułu
        WS_SYSMENU     |        // przycisk X (zamknij)
        WS_MINIMIZEBOX,         // przycisk minimalizacji
        // BRAK WS_THICKFRAME → nie można zmieniać rozmiaru myszką
        // BRAK WS_MAXIMIZEBOX → brak przycisku maksymalizacji

        100, 100,              // pozycja X, Y okna na ekranie (piksele)
        800, 600,              // szerokość, wysokość okna (piksele)
        NULL,                  // okno-rodzic (NULL = brak rodzica)
        NULL,                  // menu (NULL = brak)
        hInst,                 // identyfikator aplikacji
        NULL                   // dodatkowe dane (NULL = nie używamy)
    );

    ShowWindow(hwnd, nCmdShow); // pokaż okno (normalnie/zminimalizowane/etc)
    UpdateWindow(hwnd);         // wymuś natychmiastowe narysowanie okna

    // ── Pętla komunikatów ───────────────────────────────
    // GetMessage czeka na komunikat od Windowsa i wkłada go do msg
    // Zwraca FALSE gdy dostanie WM_QUIT → pętla się kończy → program zamyka
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // tłumaczy WM_KEYDOWN → WM_CHAR (dla tekstu)
        DispatchMessage(&msg);  // wysyła komunikat do WndProc
    }
    return (int)msg.wParam;     // kod wyjścia programu
}

// ── Funkcja obsługi komunikatów ─────────────────────────
// Windows wywołuje tę funkcję gdy coś się dzieje z oknem
// hwnd   = które okno dostało komunikat
// msg    = co się stało (WM_DESTROY, WM_KEYDOWN...)
// wParam = dodatkowa dana 1 (zależy od msg)
// lParam = dodatkowa dana 2 (zależy od msg)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_DESTROY:         // użytkownik kliknął X
            PostQuitMessage(0); // wyślij WM_QUIT → GetMessage zwróci FALSE
            return 0;           // zwróć 0 = obsłużyłem ten komunikat
    }

    // Wszystkie komunikaty których nie obsługujesz
    // oddaj Windowsowi — on wie co z nimi zrobić
    // (minimalizacja, przeciąganie okna, etc.)
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ── Co możesz zmieniać ──────────────────────────────────
// RGB(30,80,120)     → zmień kolor tła
// L"Moje pierwsze okno" → zmień tytuł
// 100, 100           → zmień startową pozycję okna
// 800, 600           → zmień rozmiar okna
// Dodaj WS_THICKFRAME → okno będzie resizowalne
// Dodaj WS_MAXIMIZEBOX → pojawi się przycisk maksymalizacji
