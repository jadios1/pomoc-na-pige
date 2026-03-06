#include "app_2048.h"

// ============================================================
// Inicjalizacja statycznego pola klasy
// Robimy to poza klasą, w pliku .cpp
// std::wstring const app_2048::s_class_name  znaczy:
// "pole s_class_name, należące do klasy app_2048, typu const wstring"
// ============================================================
std::wstring const app_2048::s_class_name{ L"2048 Window" };


// ============================================================
// KONSTRUKTOR
// Wywołuje się gdy piszesz: app_2048 app{ hInstance };
// Lista po dwukropku to "inicjalizacja pól" — ustawiamy wartości startowe
// ============================================================
app_2048::app_2048(HINSTANCE instance)
    : m_instance{ instance }   // zapisz "bilet" do programu
    , m_main{}                 // na razie nullptr, uzupełnimy za chwilę
    , m_popup{}                // na razie nullptr
{
    // Krok 1: Zarejestruj klasę okna u Windowsa
    register_class();

    // Style głównego okna:
    // WS_OVERLAPPED  — podstawowe okno (bez caption i border domyślnie)
    // WS_CAPTION     — pasek tytułu z tekstem
    // WS_SYSMENU     — menu systemowe + przycisk X
    // WS_MINIMIZEBOX — przycisk minimalizuj
    // BRAK WS_SIZEBOX i WS_MAXIMIZEBOX — okno nie może być resize'owane ani maksymalizowane
    DWORD main_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    // Style drugiego (pomocniczego) okna:
    // Bez WS_SYSMENU i WS_MINIMIZEBOX — brak przycisku X i minimalizuj
    DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;

    // Krok 2: Stwórz oba okna
    m_main  = create_window(main_style);              // główne okno, bez rodzica
    m_popup = create_window(popup_style, m_main);     // drugie okno, właścicielem jest m_main
    // Uwaga: okno z właścicielem (owner) NIE pojawia się na pasku zadań!
    // To dlatego przekazujemy m_main jako parent dla m_popup
}


// ============================================================
// REJESTRACJA KLASY OKNA
// Mówimy Windowsowi: "takie okno będę tworzył, oto jego opis"
// Robi się to RAZ — potem można tworzyć wiele okien tej klasy
// ============================================================
bool app_2048::register_class()
{
    // Sprawdź czy klasa już zarejestrowana (np. przy ponownym wywołaniu)
    WNDCLASSEXW desc{};
    if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0)
        return true; // już jest, nie rejestruj ponownie

    // Wypełnij opis klasy okna
    desc = {
        .cbSize      = sizeof(WNDCLASSEXW),  // rozmiar struktury — wymagane przez WinAPI
        .lpfnWndProc = window_proc_static,    // KTÓRA funkcja obsługuje wiadomości tego okna
        .hInstance   = m_instance,            // do którego programu należy ta klasa
        .hCursor     = LoadCursorW(nullptr, IDC_ARROW), // kursor myszy — zwykła strzałka
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),   // kolor tła — systemowy biały
        .lpszClassName = s_class_name.c_str()           // nazwa klasy (używasz jej przy CreateWindowExW)
    };

    // Zarejestruj u Windowsa — zwraca 0 jeśli błąd
    return RegisterClassExW(&desc) != 0;
}


// ============================================================
// TWORZENIE OKNA
// Na podstawie zarejestrowanej klasy tworzymy konkretne okno
// style  — jakie elementy ma mieć to konkretne okno
// parent — właściciel okna (nullptr = okno główne, pojawi się na pasku zadań)
// ============================================================
HWND app_2048::create_window(DWORD style, HWND parent)
{
    return CreateWindowExW(
        0,                        // rozszerzone style — na razie brak
        s_class_name.c_str(),     // nazwa klasy którą zarejestrowaliśmy
        L"2048",                  // tekst na pasku tytułu
        style,                    // style okna (przekazane z konstruktora)
        CW_USEDEFAULT,            // pozycja X — Windows sam wybierze
        0,                        // pozycja Y — ignorowana gdy X to CW_USEDEFAULT
        500,                      // szerokość okna w pikselach
        500,                      // wysokość okna w pikselach
        parent,                   // właściciel okna (nullptr lub m_main)
        nullptr,                  // menu — brak
        m_instance,               // "bilet" do programu
        this);                    // ← WAŻNE: przekazujemy wskaźnik na obiekt klasy
                                  // Windows zapamięta go i przekaże do WndProc przy WM_NCCREATE
}


// ============================================================
// STATYCZNA WINDOW PROCEDURE
//
// Problem: Windows woła WndProc bez wiedzy o naszym obiekcie klasy.
// Funkcja składowa klasy (metoda) nie może być przekazana do WinAPI
// bo ma ukryty parametr `this`.
//
// Rozwiązanie: statyczna funkcja (nie ma `this`) która:
// 1. Przy tworzeniu okna (WM_NCCREATE) — wyciąga wskaźnik `this`
//    z parametru lparam i "przykleja" go do okna przez SetWindowLongPtrW
// 2. Przy każdej kolejnej wiadomości — wyciąga ten wskaźnik przez GetWindowLongPtrW
//    i wywołuje właściwą metodę obiektu: app->window_proc(...)
// ============================================================
LRESULT CALLBACK app_2048::window_proc_static(
    HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    app_2048* app = nullptr;

    if (message == WM_NCCREATE)
    {
        // WM_NCCREATE — pierwsza wiadomość przy tworzeniu okna
        // lparam to wskaźnik na strukturę CREATESTRUCTW
        // w jej polu lpCreateParams jest nasz wskaźnik `this` (przekazany przez CreateWindowExW)
        app = (app_2048*)((LPCREATESTRUCTW)lparam)->lpCreateParams;

        // "Przyklej" wskaźnik do okna — GWLP_USERDATA to specjalne miejsce
        // na dane użytkownika, każde okno ma takie miejsce
        SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)app);
    }
    else
    {
        // Dla wszystkich innych wiadomości — wyciągnij wcześniej przyklejony wskaźnik
        app = (app_2048*)GetWindowLongPtrW(window, GWLP_USERDATA);
    }

    // Przekaż wiadomość do metody obiektu (jeśli wskaźnik istnieje)
    // Jeśli nie — oddaj do domyślnej obsługi Windowsa
    LRESULT result = app
        ? app->window_proc(window, message, wparam, lparam)
        : DefWindowProcW(window, message, wparam, lparam);

    // Przy niszczeniu okna — wyczyść wskaźnik żeby nie było wiszących referencji
    if (message == WM_NCDESTROY)
        SetWindowLongPtrW(window, GWLP_USERDATA, 0);

    return result;
}


// ============================================================
// WŁAŚCIWA OBSŁUGA WIADOMOŚCI
// Tu piszemy co ma się dziać gdy użytkownik coś kliknie, zamknie okno, itd.
// ============================================================
LRESULT app_2048::window_proc(
    HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CLOSE:
        // Użytkownik kliknął X — zniszcz główne okno
        // (to wywoła WM_DESTROY dla m_main)
        DestroyWindow(m_main);
        return 0;

    case WM_DESTROY:
        // Okno zostało zniszczone
        // Kończymy pętlę wiadomości tylko gdy zniszczone jest GŁÓWNE okno
        // (nie chcemy kończyć programu gdy zamknięte jest tylko pomocnicze)
        if (window == m_main)
            PostQuitMessage(EXIT_SUCCESS); // wyślij WM_QUIT → pętla się kończy
        return 0;
    }

    // Wszystkie wiadomości których nie obsługujemy — oddaj Windowsowi
    return DefWindowProcW(window, message, wparam, lparam);
}


// ============================================================
// PĘTLA WIADOMOŚCI
// Program "żyje" tutaj — czeka na zdarzenia i je obsługuje
// Kończy się gdy GetMessageW dostanie WM_QUIT (z PostQuitMessage)
// ============================================================
int app_2048::run(int show_command)
{
    // Pokaż oba okna
    ShowWindow(m_main, show_command); // główne — normalnie (wg show_command)
    ShowWindow(m_popup, SW_SHOWNA);   // pomocnicze — pokaż BEZ aktywowania
                                      // (SW_SHOWNA = Show No Activate)
                                      // dzięki temu główne okno zostaje aktywne

    MSG msg{};
    BOOL result = TRUE;

    // GetMessageW zwraca:
    //   > 0  — dostała wiadomość, idź dalej
    //   = 0  — dostała WM_QUIT, kończymy
    //   = -1 — błąd
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
    {
        if (result == -1)
            return EXIT_FAILURE; // błąd — wyjdź z kodem błędu

        TranslateMessage(&msg);  // tłumaczy klawisze na znaki (WM_CHAR)
        DispatchMessageW(&msg);  // wysyła wiadomość do odpowiedniego WndProc
    }

    return EXIT_SUCCESS; // wszystko ok, wróć 0
}
