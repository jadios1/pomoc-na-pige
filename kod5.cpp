#include <windows.h>

HWND         hKwadrat;
int          kwX, kwY;          // aktualna pozycja kwadratu
const int    ROZMIAR = 80;      // rozmiar kwadratu — const = nie zmienia się
const int    KROK    = 8;       // ile pikseli na jedno wciśnięcie
HBRUSH       gPedzel;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {

    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"WASDOkno";
    wc.hbrBackground = CreateSolidBrush(RGB(25, 25, 25));
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        L"WASDOkno", L"WASD lub strzalki zeby ruszac kwadratem",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        200, 100, 700, 500,
        NULL, NULL, hInst, NULL
    );

    // oblicz środek okna żeby tam zacząć
    RECT r;
    GetClientRect(hwnd, &r);           // r.right = szerokość, r.bottom = wysokość
    kwX = (r.right  - ROZMIAR) / 2;   // (szerokość - rozmiar) / 2 = środek X
    kwY = (r.bottom - ROZMIAR) / 2;   // (wysokość  - rozmiar) / 2 = środek Y

    hKwadrat = CreateWindow(
        L"STATIC", L"",
        WS_CHILD | WS_VISIBLE,
        kwX, kwY,
        ROZMIAR, ROZMIAR,
        hwnd, NULL, hInst, NULL
    );

    gPedzel = CreateSolidBrush(RGB(50, 220, 100));  // zielony

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

        case WM_KEYDOWN: {
            RECT r;
            GetClientRect(hwnd, &r);

            switch (wParam) {
                case VK_LEFT:  case 'A': kwX -= KROK; break;  // X maleje = ruch w lewo
                case VK_RIGHT: case 'D': kwX += KROK; break;  // X rośnie = ruch w prawo
                case VK_UP:    case 'W': kwY -= KROK; break;  // Y maleje = ruch w górę (oś Y odwrócona!)
                case VK_DOWN:  case 'S': kwY += KROK; break;  // Y rośnie = ruch w dół
                default: return 0;  // inny klawisz: nic nie rób
            }

            // ogranicz do granic okna — bez tego kwadrat wyjdzie za ekran
            if (kwX < 0)                    kwX = 0;
            if (kwY < 0)                    kwY = 0;
            if (kwX + ROZMIAR > r.right)    kwX = r.right  - ROZMIAR;
            if (kwY + ROZMIAR > r.bottom)   kwY = r.bottom - ROZMIAR;

            SetWindowPos(hKwadrat, NULL, kwX, kwY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            if ((HWND)lParam == hKwadrat) {
                return (LRESULT)gPedzel;
            }
            break;
        }

        case WM_DESTROY:
            if (gPedzel) DeleteObject(gPedzel);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Co możesz zmieniać:
// const int ROZMIAR = 80   → rozmiar kwadratu
// const int KROK    = 8    → prędkość (piksele na klawisz)
// RGB(50,220,100)          → kolor kwadratu
// Usuń blok if-ów granic   → kwadrat wychodzi za ekran
//
// Jak zrobić płynny ruch (jak w lab 2):
// 1. SetTimer(hwnd, 1, 16, NULL) w WinMain
// 2. W WM_KEYDOWN zapisuj tylko: dx = -KROK (zamiast ruszać od razu)
// 3. W WM_TIMER: kwX += dx; kwY += dy; SetWindowPos(...);
