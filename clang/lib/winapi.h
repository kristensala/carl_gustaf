#pragma once

#include <stddef.h>
#include <wchar.h>

#ifdef _WIN32
  #ifdef WINAPI_BUILD
    #define WINAPI_API __declspec(dllexport)
  #else
    #define WINAPI_API __declspec(dllimport)
  #endif
#else
  #define WINAPI_API
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

WINAPI_API int list_store_applications(AppEntryList* result);
WINAPI_API void free_store_applications(AppEntryList* data);
WINAPI_API int launch_entry(AppEntry entry); // no need 

#ifdef __cplusplus
}
#endif

