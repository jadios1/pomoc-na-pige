#include <windows.h>

HWND      hKwadrat = NULL;  // uchwyt kwadratu, NULL = jeszcze nie istnieje
HINSTANCE gHInst;           // identyfikator aplikacji
HBRUSH    gPedzel  = NULL;  // pędzel (kolor) — globalny żeby nie tworzyć co odświeżenie

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    gHInst = hInst;

    WNDCLASS wc      = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"KlikOkno";
    wc.hbrBackground = CreateSolidBrush(RGB(20, 20, 35));  // ciemnogranatowe tło
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        L"KlikOkno", L"Kliknij myszka zeby pojawil sie kwadrat",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        200, 100, 700, 500,
        NULL, NULL, hInst, NULL
    );

    gPedzel = CreateSolidBrush(RGB(80, 180, 255));  // jasnoniebieski — tworzysz raz!

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

        case WM_LBUTTONDOWN: {   // lewy przycisk myszy wciśnięty
            // lParam zawiera pozycję kursora spakowaną w jednej liczbie
            // LOWORD = niższe 16 bitów = X
            // HIWORD = wyższe 16 bitów = Y
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            // wyśrodkuj kwadrat na kursorze: lewy górny róg = kursor - połowa rozmiaru
            int rozmiar = 60;
            int kwX = mouseX - rozmiar / 2;
            int kwY = mouseY - rozmiar / 2;

            if (hKwadrat == NULL) {
                // pierwsze kliknięcie: stwórz kwadrat
                hKwadrat = CreateWindow(
                    L"STATIC", L"",
                    WS_CHILD | WS_VISIBLE,
                    kwX, kwY,
                    rozmiar, rozmiar,
                    hwnd, NULL, gHInst, NULL
                );
            } else {
                // kolejne kliknięcia: przesuń istniejący (taniej niż niszczyć i tworzyć)
                SetWindowPos(hKwadrat, NULL, kwX, kwY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            if ((HWND)lParam == hKwadrat) {
                return (LRESULT)gPedzel;  // zwróć globalny pędzel
            }
            break;
        }

        case WM_DESTROY:
            if (gPedzel) DeleteObject(gPedzel);  // zwolnij pędzel przed zamknięciem
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Co możesz zmieniać:
// int rozmiar = 60           → rozmiar kwadratu
// RGB(80,180,255)            → kolor kwadratu
// WM_LBUTTONDOWN → WM_MOUSEMOVE  → kwadrat śledzi kursor na żywo!
// WM_LBUTTONDOWN → WM_RBUTTONDOWN → reaguje na prawy przycisk
