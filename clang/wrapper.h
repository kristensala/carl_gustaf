#pragma once
#include <windows.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    std::string displayName;
    winrt::Windows::ApplicationModel::Core::AppListEntry entry;
} AppEntry;

typedef BOOL (CALLBACK *package_callback)(
    const wchar_t* full_name,
    const wchar_t* display_name,
    void* context);

/* Returns HRESULT. Callback returning FALSE stops enumeration. */
__declspec(dllimport)
HRESULT WINAPI EnumerateCurrentUserPackages(
    package_callback callback,
    void* context);

#ifdef __cplusplus
}
#endif
