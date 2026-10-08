#include <iostream>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Management.Deployment.h>

#include <vector>
using std::vector;

// id?
struct Application {
    std::string displayName;
    winrt::Windows::ApplicationModel::Core::AppListEntry entry;
};

int main() {
    try {
        winrt::init_apartment();
        winrt::Windows::Management::Deployment::PackageManager manager;
        winrt::hstring target = L"Microsoft.ScreenSketch_8wekyb3d8bbwe!App"; // snipping tool

        vector<Application> applications;

        for (auto const& package : manager.FindPackagesForUser(L"")) {
            std::string displayName = winrt::to_string(package.DisplayName());
            Application application{ displayName, NULL };
            
            // each entry has LaunchAsync()
            for (auto const& entry : package.GetAppListEntries()) {
                application.entry = entry;

                /*std::wcout << entry.AppUserModelId().c_str() << L"\n";
                std::wcout
                    << package.DisplayName().c_str()
                    << L"\n  " << package.Id().FullName().c_str()
                    << L"\n\n";*/

                //if (entry.AppUserModelId().c_str() == target) {

                    /*bool ok = entry.LaunchAsync().get();
                    std::wcout << (ok ? L"Launched\n" : L"Launch failed\n");
                    winrt::uninit_apartment();*/
                //}
            }

            applications.push_back(application);
        }

        for (auto const& app : applications) {
            std::wcout
                << app.displayName.c_str()
                << L"\n";
        }


        winrt::uninit_apartment();
        return 0;
    }
    catch (winrt::hresult_error const& e) {
        std::cerr << "WinRT error: 0x"
                  << std::hex << static_cast<unsigned long>(e.code())
                  << "\n";
        return 1;
    }
}

// @todo
static vector<Application> GetInstalledApplications() {
    vector<Application> applications;

    try {
        winrt::init_apartment();
        winrt::Windows::Management::Deployment::PackageManager manager;
        winrt::hstring target = L"Microsoft.ScreenSketch_8wekyb3d8bbwe!App"; // snipping tool


        for (auto const& package : manager.FindPackagesForUser(L"")) {
            std::string displayName = winrt::to_string(package.DisplayName());
            Application application{ displayName, NULL };
            
            // each entry has LaunchAsync()
            // Why multiple entries?
            for (auto const& entry : package.GetAppListEntries()) {
                application.entry = entry;
            }

            applications.push_back(application);
        }

        winrt::uninit_apartment();
        return applications;
    }
    catch (winrt::hresult_error const& e) {
        std::cerr << "WinRT error: 0x"
                  << std::hex << static_cast<unsigned long>(e.code())
                  << "\n";

        winrt::uninit_apartment();
        return applications;
    }
}

//https://learn.microsoft.com/en-us/uwp/api/windows.applicationmodel.package?view=winrt-28000
// @todo
static bool LaunchApplication(Application app) {
    return app.entry.LaunchAsync().get();
}
