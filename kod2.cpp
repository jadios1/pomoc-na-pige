// ═══════════════════════════════════════════════════════
// KOD 2 — dwa okna: ruszasz jedno, drugie idzie za nim
// Kwadrat (dziecko) jest przyklejony do głównego okna
// Gdy główne okno się rusza → kwadrat rusza się razem
// Gdy wciskasz strzałki → główne okno się przesuwa
// ═══════════════════════════════════════════════════════
#include <windows.h>

// ── Zmienne globalne ────────────────────────────────────
HWND hKwadrat;   // uchwyt do kwadratu (dziecięce okno)
                 // globalna bo WndProc i WinMain muszą ją znać

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {

    // ── Rejestracja klasy głównego okna ─────────────────
    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"GlowneOkno";
    wc.hbrBackground = CreateSolidBrush(RGB(40, 40, 40)); // ciemne tło
    RegisterClass(&wc);

    // ── Tworzenie głównego okna ──────────────────────────
    HWND hwnd = CreateWindow(
        L"GlowneOkno", L"Rusz mnie strzalkami!",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        300, 200,   // startowa pozycja okna
        400, 300,   // rozmiar okna
        NULL, NULL, hInst, NULL
    );

    // ── Tworzenie kwadratu ───────────────────────────────
    // L"STATIC" = wbudowana klasa Windowsa (nie trzeba rejestrować)
    // WS_CHILD   = to jest dziecko hwnd (porusza się razem z nim)
    // WS_VISIBLE = pokaż od razu
    hKwadrat = CreateWindow(
        L"STATIC", L"",          // klasa i tekst (pusty)
        WS_CHILD | WS_VISIBLE,    // styl: dziecko + widoczne
        150, 100,                 // pozycja WEWNĄTRZ głównego okna
        80,  80,                  // rozmiar kwadratu
        hwnd,                     // rodzic = główne okno ← to kluczowe!
        NULL, hInst, NULL         // brak menu, identyfikator aplikacji
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_KEYDOWN: {          // użytkownik wcisnął klawisz
            // Pobierz aktualną pozycję głównego okna na ekranie
            RECT r;
            GetWindowRect(hwnd, &r); // r.left, r.top = pozycja okna na ekranie
            //               ↑ GetWindowRect = cały ekran
            //                 GetClientRect = wnętrze okna (bez ramki)

            int x = r.left; // aktualna pozycja X okna
            int y = r.top;  // aktualna pozycja Y okna

            switch (wParam) {         // wParam = kod wciśniętego klawisza
                case VK_LEFT:  x -= 10; break; // strzałka ← : przesuń w lewo
                case VK_RIGHT: x += 10; break; // strzałka → : przesuń w prawo
                case VK_UP:    y -= 10; break; // strzałka ↑ : przesuń w górę
                case VK_DOWN:  y += 10; break; // strzałka ↓ : przesuń w dół
            }

            // Przesuń główne okno na nową pozycję
            // Kwadrat (hKwadrat) jest dzieckiem → automatycznie idzie razem!
            SetWindowPos(
                hwnd,           // które okno przesuwamy
                NULL,           // z-order (NULL = bez zmiany)
                x, y,           // nowa pozycja
                0, 0,          // rozmiar — ignorowany przez SWP_NOSIZE
                SWP_NOSIZE | SWP_NOZORDER
                // SWP_NOSIZE   = nie zmieniaj rozmiaru
                // SWP_NOZORDER = nie zmieniaj kolejności okien
            );
            return 0;
        }

        // WM_CTLCOLORSTATIC: Windows pyta "jaki kolor ma mieć ta kontrolka STATIC?"
        // Wysyłane do RODZICA gdy ma narysować dziecko-STATIC
        case WM_CTLCOLORSTATIC: {
            // lParam = HWND kontrolki która pyta o kolor
            // Sprawdzamy czy to nasz kwadrat (bo może być wiele STATIC)
            if ((HWND)lParam == hKwadrat) {
                // Zwróć pędzel z kolorem — Windows użyje go do wypełnienia kwadratu
                return (LRESULT)CreateSolidBrush(RGB(255, 100, 0)); // pomarańczowy
                // ⚠️ Uproszczenie: tutaj tworzymy nowy pędzel przy każdym
                // odświeżeniu. W prawdziwym kodzie trzymaj go w zmiennej
                // globalnej i kasuj starą przed stworzeniem nowej.
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ── Co możesz zmieniać ──────────────────────────────────
// 10 w x±=10/y±=10     → prędkość przesuwania okna
// 150, 100 (pozycja kwadratu) → gdzie kwadrat siedzi wewnątrz okna
// 80, 80 (rozmiar kwadratu)   → rozmiar kwadratu
// RGB(255,100,0) → kolor kwadratu
// Zamień VK_LEFT/VK_RIGHT na 'A'/'D' → sterowanie WASD
