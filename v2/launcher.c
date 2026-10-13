#include "launcher.h"
#include <windows.h>
#include <stdio.h>
#include <objbase.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "winlib.h"
#include "tinkers.h"

const char g_szClassName[] = "myWindowClass";

const int WindowWidth = 1000;
const int WindowHeight = 300;

AppState g_app_state = {
    .input = {0},
    .input_len = 0,
    .applications = NULL,
    .application_count = 0,
    .application_capacity = 0,
    .search_results = NULL,
    .search_results_count = 0,
    .selected_application = NULL,
    .selected_application_idx = 0
};

// example: https://mouri.moe/en/2021/11/07/Launch-Windows-Store-App-via-Win32-API/
// if has AUMID use that to launch the application else just execute the executable
HRESULT launch_application(Application *app) {
    // https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-iapplicationactivationmanager-activateapplication
    /*
       HRESULT ActivateApplication(
       [in]  LPCWSTR         appUserModelId,
       [in]  LPCWSTR         arguments,
       [in]  ACTIVATEOPTIONS options,
       [out] DWORD           *processId
       );
    */
}

void append_to_input(char key) {
    if (g_app_state.input_len < 50) {
        g_app_state.input[g_app_state.input_len] = key;
        g_app_state.input_len++;

        g_app_state.input[g_app_state.input_len] = '\0';
    }
}

// pop
void remove_from_input() {
    if (g_app_state.input_len > 0) {
        g_app_state.input_len--;
        g_app_state.input[g_app_state.input_len] = '\0';
    }
}

// @todo: scan start menu and other common places where application are installed
// win32 example: https://learn.microsoft.com/en-us/windows/win32/fileio/listing-the-files-in-a-directory
//
// START_MENU_PORGRAMS_PATH :: "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\*"
// PATH :: "\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\*"
void scan_system_for_applications() {
    /*WIN32_FIND_DATA file_data;
    HANDLE search_handle = FindFirstFileW("todo", &file_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        printf("Failed to read dir\n");
        return;
    }

    do {
        // @todo
    } while (FindNextFileW(search_handle, &file_data));*/
}

void draw_application_list(HDC hdc) {
    int pos_y = 25;
    
    // @todo: find a better way
    // create once and Delete on application exit
    HBRUSH blue_brush = CreateSolidBrush(RGB(0, 0, 255));

    for (size_t i = 0; i < g_app_state.search_results_count; ++i) {
        Application *app = g_app_state.search_results[i];

        RECT rect = {5, pos_y, WindowWidth, pos_y + 20};

        if (i == g_app_state.selected_application_idx) {
            FillRect(hdc, &rect, blue_brush);
        }

        DrawTextA(hdc, app->name, strlen(app->name), &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);

        pos_y += 20;

        if (pos_y > WindowHeight - 20) {
            break;
        }
    }

    DeleteObject(blue_brush);
}

void update_search_results() {
    g_app_state.search_results_count = 0;

    char *input_lower = string_to_lower(g_app_state.input);
    for (size_t i = 0; i < g_app_state.application_count; ++i) {
        Application *app = &g_app_state.applications[i];

        char *app_name_lower = string_to_lower(app->name);

        if (strstr(app_name_lower, input_lower) != NULL) {
            g_app_state.search_results[g_app_state.search_results_count++] = app;
        }

        free(app_name_lower);
    }

    if (g_app_state.search_results_count > 0) {
        g_app_state.selected_application = g_app_state.search_results[0];
        g_app_state.selected_application_idx = 0;
    }

    free(input_lower);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
        break;
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
            char c = (WCHAR)wParam;
            int should_update_search = 0;

            if (c == 8) { // Backspace
                remove_from_input();
                should_update_search = 1;
            } else if ((int)wParam >= 32) {
                append_to_input(c);
                should_update_search = 1;
            }

            if (should_update_search) {
                update_search_results();

                // Force redraw
                InvalidateRect(hwnd, NULL, true);
            }

            return 0;
        case WM_KEYDOWN: 
            if (wParam == VK_ESCAPE) {
                PostMessageA(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            if (wParam == VK_UP) {
                if (g_app_state.selected_application_idx > 0) {
                    g_app_state.selected_application_idx--;
                    InvalidateRect(hwnd, NULL, true);
                }
                return 0;
            }
            if (wParam == VK_DOWN) {
                if (g_app_state.selected_application_idx < g_app_state.search_results_count) {
                    g_app_state.selected_application_idx++;
                    InvalidateRect(hwnd, NULL, true);
                }
                return 0;
            }

            if (wParam == VK_RETURN) {
                // @todo: launch the selected application
            }
            return 0;
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            RECT rect;
            GetClientRect(hwnd, &rect);

            //RECT search_results_rect = {}; // Invalidate this on search update

            rect.top += 5;
            rect.left += 5;

            SetTextColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, TRANSPARENT);

            // input text
            DrawTextA(hdc, g_app_state.input, -1, &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);

            // @todo draw application list
            draw_application_list(hdc);

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

    // @note(ksala): winrt::init_apartment() is triggered in C++ side and this here crashed the program
    // maybe init and uninit here and remove from C++ side
    /*HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        printf("CoInitializeEx failed\n");
        return 1;
    }*/

    hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            g_szClassName,
            NULL,
            WS_POPUPWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight,
            NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    AppEntryList apps = {0};

    // @todo: set the search_results_rect
    //search_results_rect = {0, 30, WindowWidth, WindowHeight - 30};

    int result = list_store_applications(&apps); // copy to a state and then free
    if (result == 0) {
        g_app_state.applications = calloc(apps.count, sizeof(Application));

        for (size_t i = 0; i < apps.count; ++i) {
            Application application = {
                .name = NULL,
                .path_to_exe = NULL,
                .is_store_application = 1,
                .aumid = NULL
            };

            size_t len = strlen(apps.items[i].display_name);
            application.name = malloc(len + 1);
            if (application.name != NULL) {
                strcpy(application.name, apps.items[i].display_name);
            }

            if (apps.items[i].app_user_model_id != NULL) {
                size_t aumid_len = strlen(apps.items[i].app_user_model_id);
                application.aumid = malloc(aumid_len + 1);
                if (application.aumid != NULL) {
                    strcpy(application.aumid, apps.items[i].app_user_model_id);
                }
            }

            g_app_state.applications[g_app_state.application_count] = application;
            g_app_state.application_count++;

            /*printf("%s: ", application.name);
            printf("%s\n", application.aumid);*/
        }

        free_store_applications(&apps);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    cleanup:
        //CoUninitialize();
        free_store_applications(&apps);

        for (size_t i = 0; i < g_app_state.application_count; ++i) {
            free(g_app_state.applications[i].name);
            g_app_state.applications[i].name = NULL;

            free(g_app_state.applications[i].aumid);
            g_app_state.applications[i].aumid = NULL;
        }

        free(g_app_state.applications);
        g_app_state.applications = NULL;
        g_app_state.application_count = 0;

    return Msg.wParam;
}
