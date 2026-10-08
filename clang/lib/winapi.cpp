#include <iostream>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Management.Deployment.h>

#include "winapi.h"

struct AppEntryImpl {
    winrt::Windows::ApplicationModel::Core::AppListEntry entry;
};

extern "C" int list_store_applications(AppEntryList* result) {
    if (!result) {
        return -1;
    }

    *result = {};

    try {
        winrt::init_apartment();
        winrt::Windows::Management::Deployment::PackageManager manager;

        std::vector<AppEntryInfo> applications;

        for (auto const& package : manager.FindPackagesForUser(L"")) {
            auto display_name = package.DisplayName();

            const wchar_t* source = display_name.c_str();
            auto* copy = new wchar_t[display_name.size() + 1];
            std::wmemcpy(copy, source, display_name.size());
            copy[display_name.size()] = L'\0';

            AppEntryInfo app_info{NULL, copy};

            for (auto const& entry : package.GetAppListEntries()) {
                auto* e = new AppEntryImpl{ entry };
                app_info.entry = e;
                applications.push_back(app_info);
                break;
            }
        }

        // this needs to be deleted later
        auto* data = new AppEntryInfo[applications.size()]{};
        std::copy(applications.begin(), applications.end(), data);

        result->items = data;
        result->count = applications.size();
    } catch (winrt::hresult_error const& e) {
        winrt::uninit_apartment();
        return -1;
    }

    winrt::uninit_apartment();
    return 0;
}

extern "C" void free_store_applications(AppEntryList* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; ++i) {
        delete list->items[i].entry;
        delete[] list->items[i].display_name;
    }

    delete[] list->items;
    *list = {};
}

