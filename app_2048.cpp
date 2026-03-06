#include "app_2048.h"

std::wstring const app_2048::s_class_name{ L"2048 Window" };


// ============================================================
// KONSTRUKTOR
// ============================================================
app_2048::app_2048(HINSTANCE instance)
    : m_instance{ instance }
    , m_main{}
    , m_popup{}
    , m_screen_size{
        GetSystemMetrics(SM_CXSCREEN),  // szerokość ekranu w pikselach
        GetSystemMetrics(SM_CYSCREEN)   // wysokość ekranu w pikselach
      }
{
    register_class();

    DWORD main_style  = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;

    m_main  = create_window(main_style);
    m_popup = create_window(popup_style, m_main);
    // okno z właścicielem (m_main) NIE pojawia się na pasku zadań
}


// ============================================================
// REJESTRACJA KLASY OKNA
// ============================================================
bool app_2048::register_class()
{
    WNDCLASSEXW desc{};
    if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0)
        return true;

    desc = {
        .cbSize        = sizeof(WNDCLASSEXW),
        .lpfnWndProc   = window_proc_static,
        .hInstance     = m_instance,
        .hCursor       = LoadCursorW(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszClassName = s_class_name.c_str()
    };

    return RegisterClassExW(&desc) != 0;
}


// ============================================================
// TWORZENIE OKNA
// ============================================================
HWND app_2048::create_window(DWORD style, HWND parent)
{
    return CreateWindowExW(
        0,
        s_class_name.c_str(),
        L"2048",
        style,
        CW_USEDEFAULT, 0,
        500, 500,
        parent,
        nullptr,
        m_instance,
        this);   // przekazujemy wskaznik na obiekt - wyciągniemy go w WM_NCCREATE
}


// ============================================================
// STATYCZNA WINDOW PROCEDURE
//
// Windows nie wie o naszej klasie C++, wola zwykla funkcje.
// Ta funkcja "przykleja" wskaznik na obiekt do okna przy tworzeniu,
// a potem go wyciaga dla kazdej kolejnej wiadomosci
// i przekazuje do prawdziwej metody obiektu.
// ============================================================
LRESULT CALLBACK app_2048::window_proc_static(
    HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    app_2048* app = nullptr;

    if (message == WM_NCCREATE)
    {
        // lparam = wskaznik na CREATESTRUCTW, w polu lpCreateParams jest nasz `this`
        app = (app_2048*)((LPCREATESTRUCTW)lparam)->lpCreateParams;
        // "Przyklej" wskaznik do okna w miejscu GWLP_USERDATA
        SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)app);
    }
    else
    {
        // Wyciagnij wczesniej przyklejony wskaznik
        app = (app_2048*)GetWindowLongPtrW(window, GWLP_USERDATA);
    }

    LRESULT result = app
        ? app->window_proc(window, message, wparam, lparam)
        : DefWindowProcW(window, message, wparam, lparam);

    // Przy niszczeniu — wyczysc wskaznik
    if (message == WM_NCDESTROY)
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);

    return result;
}


// ============================================================
// WŁAŚCIWA OBSŁUGA WIADOMOŚCI
// ============================================================
LRESULT app_2048::window_proc(
    HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(m_main);
        return 0;

    case WM_DESTROY:
        if (window == m_main)
            PostQuitMessage(EXIT_SUCCESS);
        return 0;

    case WM_WINDOWPOSCHANGED:
        // Windows wysyla to gdy okno zmieni pozycje LUB rozmiar
        // lparam = wskaznik na WINDOWPOS ze szczegolami co sie zmienilo
        on_window_move(window, reinterpret_cast<LPWINDOWPOS>(lparam));
        return 0;
    }

    return DefWindowProcW(window, message, wparam, lparam);
}


// ============================================================
// SYMETRYCZNY RUCH OKIEN
//
// Matematyka:
//   srodek okna ktore sie ruszylo  = jego_pozycja + jego_rozmiar/2
//   srodek drugiego okna (symetria) = rozmiar_ekranu - srodek_pierwszego
//   nowa pozycja drugiego           = srodek_drugiego - rozmiar_drugiego/2
//
// Przyklad (ekran 1920x1080, okna 500x500):
//   Okno A przesuniete na (200, 100)
//   srodek_A  = (450, 350)
//   srodek_B  = (1920-450, 1080-350) = (1470, 730)
//   pozycja_B = (1470-250, 730-250)  = (1220, 480)
// ============================================================
void app_2048::on_window_move(HWND window, LPWINDOWPOS params)
{
    // Znajdz drugie okno
    HWND other = (window == m_main) ? m_popup : m_main;

    // Pobierz aktualny prostokat drugiego okna {left, top, right, bottom}
    RECT other_rc;
    GetWindowRect(other, &other_rc);

    // Wylicz rozmiar drugiego okna
    SIZE other_size{
        .cx = other_rc.right  - other_rc.left,
        .cy = other_rc.bottom - other_rc.top
    };

    // Krok 1: srodek okna ktore sie ruszylo
    // params->x, params->y = lwy gorny rog; params->cx, params->cy = rozmiar
    POINT center_moved{
        .x = params->x + params->cx / 2,
        .y = params->y + params->cy / 2
    };

    // Krok 2: srodek drugiego okna (symetria wzgledem srodka ekranu)
    POINT center_other{
        .x = m_screen_size.x - center_moved.x,
        .y = m_screen_size.y - center_moved.y
    };

    // Krok 3: lwy gorny rog drugiego okna
    POINT new_pos{
        .x = center_other.x - other_size.cx / 2,
        .y = center_other.y - other_size.cy / 2
    };

    // Jesli juz jest na wlasciwym miejscu — nic nie rob
    // BEZ TEGO: A przesuwa B, B dostaje WM_WINDOWPOSCHANGED i przesuwa A,
    // A dostaje WM_WINDOWPOSCHANGED i przesuwa B... nieskonczona petla!
    if (new_pos.x == other_rc.left && new_pos.y == other_rc.top)
        return;

    // Przesun drugie okno
    // SWP_NOSIZE     = nie zmieniaj rozmiaru
    // SWP_NOACTIVATE = nie aktywuj (nie kradnij focusu)
    // SWP_NOZORDER   = nie zmieniaj kolejnosci (ktore okno jest "nad")
    SetWindowPos(
        other,
        nullptr,
        new_pos.x, new_pos.y,
        0, 0,
        SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
}


// ============================================================
// PĘTLA WIADOMOŚCI
// ============================================================
int app_2048::run(int show_command)
{
    ShowWindow(m_main,  show_command);  // glowne — normalnie
    ShowWindow(m_popup, SW_SHOWNA);     // pomocnicze — bez aktywowania

    MSG msg{};
    BOOL result = TRUE;

    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
    {
        if (result == -1)
            return EXIT_FAILURE;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return EXIT_SUCCESS;
}
