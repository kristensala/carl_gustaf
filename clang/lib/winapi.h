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

// Opaque handle for winrt::Windows::ApplicationModel::Core::AppListEntry entry
typedef struct AppEntryImpl* AppEntry;

typedef struct {
    AppEntry entry;
    wchar_t* display_name;
} AppEntryInfo;

typedef struct {
    AppEntryInfo* items;
    size_t count;
} AppEntryList;

WINAPI_API int list_store_applications(AppEntryList* result);
WINAPI_API void free_store_applications(AppEntryList* data);
WINAPI_API int launch_entry(AppEntry entry);

#ifdef __cplusplus
}
#endif

