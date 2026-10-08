#include "state.h"
#include <windows.h>
#include <stdio.h>
#include "winapi.h"

const char g_szClassName[] = "myWindowClass";

const int WindowWidth = 1000;

AppState g_app_state = {
    .input = {0},
    .input_len = 0,
    .applications = NULL,
    .application_count = 0,
    .application_capacity = 0
};

void append_to_input(HWND hwnd, char key) {
    if (g_app_state.input_len < 50) {
        g_app_state.input[g_app_state.input_len] = key;
        g_app_state.input_len++;

        g_app_state.input[g_app_state.input_len] = '\0';
    }
}

void remove_from_input(HWND hwnd) {
    if (g_app_state.input_len > 0) {
        g_app_state.input_len--;
        g_app_state.input[g_app_state.input_len] = '\0';
    }
}

void scan_system_for_applications() {
    // @todo: cpp impl here plus scan common directories
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SETFOCUS: // and KILLFOCUS should destroy the caret
                          // @todo: CreateCaret api
            return 0;
        case WM_CHAR:
            char c = (WCHAR) wParam;
            if (c == 8) { // Backspace
                remove_from_input(hwnd);
            } else {
                append_to_input(hwnd, c);
            }

            // Force redraw
            InvalidateRect(hwnd, NULL, true);
            return 0;
        case WM_KEYDOWN: 
            if (wParam == VK_ESCAPE) {
                PostMessageA(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            RECT rect;
            GetClientRect(hwnd, &rect);

            rect.top += 5;
            rect.left += 5;

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);

            DrawTextA(hdc, g_app_state.input, -1, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);

            EndPaint(hwnd, &ps);

            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(
                NULL,
                "Window Registration Failed!", "Error!",
                MB_ICONEXCLAMATION | MB_OK
                );
        return 0;
    }

    hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            g_szClassName,
            NULL,
            WS_POPUPWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, 300,
            NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    AppEntryList apps = {0};

    int result = list_store_applications(&apps);
    if (result == 0) {
        for (size_t i = 0; i < apps.count; ++i) {
            printf("%ls\n", apps.items[i].display_name);
        }
    } else {
        free_store_applications(&apps);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    free_store_applications(&apps);
    

    return Msg.wParam;
}
