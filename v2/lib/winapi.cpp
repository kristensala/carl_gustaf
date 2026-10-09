#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

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
            AppEntryInfo app_info{NULL, copy, NULL};*/

            std::string utf8 = winrt::to_string(package.DisplayName());

            char *text = static_cast<char *>(std::malloc(utf8.size() + 1));
            if (text == nullptr) {
                continue;
            }

            std::memcpy(text, utf8.c_str(), utf8.size() + 1);

            AppEntryInfo app_info{ nullptr, text, nullptr };

            for (auto const& entry : package.GetAppListEntries()) {
                std::string utf8_aumid = winrt::to_string(entry.AppUserModelId());

                char *t = static_cast<char *>(std::malloc(utf8_aumid.size() + 1));
                if (t == nullptr) {
                    continue;
                }
                std::memcpy(t, utf8_aumid.c_str(), utf8_aumid.size() + 1);

                app_info.app_user_model_id = t;

                break;
            }
            applications.push_back(app_info);
        }

        auto* data = new AppEntryInfo[applications.size()]{};
        std::copy(applications.begin(), applications.end(), data);

        result->items = data;
        result->count = applications.size();
    } catch (winrt::hresult_error const& e) {
        std::fprintf(stderr, "WinRT error: 0x%08X: %s\n",
        static_cast<unsigned int>(e.code().value),
        winrt::to_string(e.message()).c_str());

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
        std::free(list->items[i].display_name);
        std::free(list->items[i].app_user_model_id);
    }

    delete[] list->items;
    *list = {};
}

