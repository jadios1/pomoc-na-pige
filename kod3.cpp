// ═══════════════════════════════════════════════════════
// KOD 3 — spacja tworzy kwadrat na górze, kwadrat spada
// Każda spacja = nowy kwadrat, wszystkie spadają naraz
// ═══════════════════════════════════════════════════════
#include <windows.h>
#include <vector>   // std::vector — lista kwadratów

// ── Struktura opisująca jeden kwadrat ───────────────────
// Każdy kwadrat ma swoją pozycję i swój HWND
struct Kwadrat {
    HWND hwnd;  // uchwyt do okna-kwadratu
    int  x, y;  // aktualna pozycja (w pikselach)
};

// ── Zmienne globalne ────────────────────────────────────
std::vector<Kwadrat> kwadraty; // lista wszystkich aktywnych kwadratów
HINSTANCE gHInst;               // identyfikator aplikacji — potrzebny do CreateWindow
                                 // globalny bo używamy go w WndProc
HWND gHwnd;                     // uchwyt głównego okna — potrzebny do GetClientRect

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    gHInst = hInst; // zapisz globalnie żeby WndProc mogło używać

    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"SpadajaceOkno";
    wc.hbrBackground = CreateSolidBrush(RGB(15, 15, 30)); // bardzo ciemne tło
    RegisterClass(&wc);

    gHwnd = CreateWindow(
        L"SpadajaceOkno", L"Spacja = nowy kwadrat!",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        200, 100, 600, 500,
        NULL, NULL, hInst, NULL
    );

    // ── Timer: co 16ms aktualizuj pozycje kwadratów ─────
    // ID timera = 1 (możesz mieć wiele timerów z różnymi ID)
    // 16ms ≈ 60 klatek na sekundę
    SetTimer(gHwnd, 1, 16, NULL);
    //        ↑     ↑  ↑   ↑
    //       okno  id  ms  NULL=użyj WM_TIMER

    ShowWindow(gHwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_KEYDOWN: {
            if (wParam == VK_SPACE) { // VK_SPACE = kod klawisza spacji

                // Pobierz szerokość okna żeby wyśrodkować kwadrat
                RECT r;
                GetClientRect(hwnd, &r);  // r.right = szerokość wnętrza okna
                //              ↑ "client" = wnętrze okna (bez paska tytułu)

                // Losowa pozycja X (żeby kwadraty nie spadały w tym samym miejscu)
                int startX = rand() % (r.right - 50); // 0 do (szerokość - 50)

                // Stwórz nowe okno-kwadrat jako dziecko głównego okna
                HWND nowy = CreateWindow(
                    L"STATIC", L"",       // klasa STATIC, bez tekstu
                    WS_CHILD | WS_VISIBLE, // dziecko, widoczne od razu
                    startX, 0,             // pozycja: losowe X, Y=0 (góra okna)
                    50, 50,               // rozmiar kwadratu 50x50
                    hwnd,                  // rodzic = główne okno
                    NULL, gHInst, NULL
                );

                // Dodaj do listy kwadratów
                kwadraty.push_back({nowy, startX, 0});
                //                 ↑     struct Kwadrat { hwnd, x, y }
            }
            return 0;
        }

        case WM_TIMER: {           // wywoływane co 16ms przez SetTimer
            if (wParam != 1) break; // sprawdź ID timera (tu używamy ID=1)

            RECT r;
            GetClientRect(hwnd, &r); // pobierz rozmiar okna (żeby wiedzieć gdzie dół)

            // Przeiteruj przez wszystkie kwadraty
            for (auto& k : kwadraty) {
                k.y += 3; // przesuń kwadrat w dół o 3 piksele
                //  ↑ y rośnie = ruch w dół (oś Y w Windows: 0 jest na górze!)

                // Przesuń okno-kwadrat na nową pozycję
                SetWindowPos(
                    k.hwnd,         // który kwadrat przesuwamy
                    NULL,           // z-order bez zmian
                    k.x, k.y,       // nowa pozycja
                    0, 0,          // rozmiar (ignorowany)
                    SWP_NOSIZE | SWP_NOZORDER
                );
            }

            // Usuń kwadraty które wypadły poza dolną krawędź
            // Iterujemy od końca żeby usuwanie nie psulo indeksów
            for (int i = (int)kwadraty.size() - 1; i >= 0; i--) {
                if (kwadraty[i].y > r.bottom) { // kwadrat poniżej dołu okna?
                    DestroyWindow(kwadraty[i].hwnd); // zniszcz okno-kwadrat
                    kwadraty.erase(kwadraty.begin() + i); // usuń z listy
                }
            }
            return 0;
        }

        // Windows pyta o kolor każdego kwadratu-STATIC
        case WM_CTLCOLORSTATIC: {
            HWND hCtrl = (HWND)lParam; // który kwadrat pyta

            // Sprawdź czy to jeden z naszych kwadratów
            for (auto& k : kwadraty) {
                if (k.hwnd == hCtrl) {
                    // Zwróć czerwony pędzel dla naszych kwadratów
                    return (LRESULT)CreateSolidBrush(RGB(220, 50, 50));
                }
            }
            break;
        }

        case WM_DESTROY:
            KillTimer(hwnd, 1); // zawsze niszcz timer przed zamknięciem!
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ── Co możesz zmieniać ──────────────────────────────────
// k.y += 3         → prędkość spadania (większa liczba = szybciej)
// SetTimer(..., 16, ...) → częstotliwość timera (mniejsza = płynniej)
// 50, 50 (rozmiar) → rozmiar kwadratów
// RGB(220,50,50)   → kolor kwadratów
// rand() % (r.right-50) → losowa X pozycja przy spawnie
// r.bottom         → próg usuwania (dół okna)
// Możesz dodać: losowy kolor dla każdego kwadratu przy tworzeniu
