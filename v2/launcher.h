#ifndef LAUNCHER_H
#define LAUNCHER_H

#define Megabytes(Value) ((Value)*1024*1024)

#include <stddef.h>
#include <windows.h>
#include "winlib.h"
#include <wchar.h>

// [[ ref: https://www.youtube.com/watch?v=tcENxzeTjbI ]]
typedef struct {
    int is_initialized;
    int permanent_storage_size; // MB
    void *permanent_storage;
} LauncherMemory;

typedef struct {
    wchar_t *name;
    wchar_t *path_to_exe;

    /* if store application use aumid else use path_to_exe*/
    int is_store_application;
    wchar_t *aumid;
} Application;

typedef struct {
    wchar_t input[50];
    size_t input_len;

    Application *applications;
    size_t application_count;
    size_t application_capacity;

    Application *selected_application;
    int selected_application_idx;
    //int current_page;

    Application *search_results[50];
    size_t search_results_count;
} AppState;

typedef struct {
    RECT cursor;
    int cursor_pos_x;
    int cursor_pos_in_input;
} Gui;

extern AppState g_app_state;
extern RECT search_results_rect;

void add_application(char *name, char *exec_path);
void free_app_state(void);
void append_to_input(char key);

#endif
