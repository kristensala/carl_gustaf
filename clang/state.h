#ifndef APP_STATE_H
#define APP_STATE_H

#include <stddef.h>
#include <windows.h>

typedef struct {
    char* name;
    char* path_to_exe;
    bool is_store_application;
} Application;

typedef struct {
    char input[50];
    size_t input_len;

    Application* applications;
    size_t application_count;
    size_t application_capacity;
} AppState;

typedef struct {
    RECT cursor;
    int cursor_pos_x;
} Gui;

extern AppState g_app_state;

void add_application(char* name, char* exec_path);
void free_app_state(void);
void append_to_input(HWND hwnd, char key);

#endif
