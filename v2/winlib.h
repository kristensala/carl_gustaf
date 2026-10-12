#pragma once

#include <stddef.h>
#include <wchar.h>

#ifdef _WIN32
  #ifdef WINLIB_BUILD
    #define WINLIB __declspec(dllexport)
  #else
    #define WINLIB __declspec(dllimport)
  #endif
#else
  #define WINLIB
#endif

#ifdef __cplusplus
extern "C" {
#endif

// @todo: get appEntry AUMID instead of the AppListEntry

// Opaque handle for winrt::Windows::ApplicationModel::Core::AppListEntry entry
typedef struct AppEntryImpl* AppEntry;

typedef struct {
    AppEntry entry;
    char *display_name;
    char *app_user_model_id; // id: used for launching the application
} AppEntryInfo;

typedef struct {
    AppEntryInfo* items;
    size_t count;
} AppEntryList;

WINLIB int list_store_applications(AppEntryList* result);
WINLIB void free_store_applications(AppEntryList* data);
WINLIB int launch_entry(AppEntry entry); // no need 

#ifdef __cplusplus
}
#endif

