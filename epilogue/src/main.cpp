#include <jansson.h>

#include <borealis.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "activity/main_activity.hpp"
#include "view/captioned_image.hpp"
#include "tab/epilogue_main.hpp"
#include "tab/settings_tab.hpp"
#include "tools/configHandler.h"
#include "tools/fsHandler.h"
#include "tools/netTasksHandler.h"

static json_error_t jerr;

using namespace brls::literals; // for _i18n

int main(const int argc, char* argv[])
{
    // We recommend to use INFO for real apps
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-d") == 0) { // Set log level
            brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);
        } else if (std::strcmp(argv[i], "-o") == 0) {
            const char* path = (i + 1 < argc) ? argv[++i] : "borealis.log";
            brls::Logger::setLogOutput(std::fopen(path, "w+"));
        } else if (std::strcmp(argv[i], "-v") == 0) {
            brls::Application::enableDebuggingView(true);
        }
    }

    brls::Platform::APP_LOCALE_DEFAULT = brls::LOCALE_AUTO;

    // Init the app and i18n
    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    brls::Application::createWindow("epilogue/title"_i18n);

    brls::Application::getPlatform()->setThemeVariant(brls::ThemeVariant::DARK);

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(false);

    // Register custom views (including tabs, which are views)
    brls::Application::registerXMLView("EpilogueMain", EpilogueMain::create);
    brls::Application::registerXMLView("SettingsTab", SettingsTab::create);
    brls::Application::registerXMLView("CaptionedImage", CaptionedImage::create);

    // Add custom values to the theme
    brls::Theme::getLightTheme().addColor("captioned_image/caption", nvgRGB(2, 176, 183));
    brls::Theme::getDarkTheme().addColor("captioned_image/caption", nvgRGB(51, 186, 227));

    // Add custom values to the style
    brls::getStyle().addMetric("about/padding_top_bottom", 50);
    brls::getStyle().addMetric("about/padding_sides", 75);
    brls::getStyle().addMetric("about/description_margin", 50);


    // Create and push the main activity to the stack
    brls::Application::pushActivity(new MainActivity());

    startHandler();

    //Update handling... WIP... stop being lazy...
    const std::string githubFetch = readFileNet("https://api.github.com/", "repos/Menhyy/Epilogue/releases/latest");
    if (githubFetch != "CURL_ERR")
    {
        char* updateNumber = nullptr;
        json_t* jshit = json_loads(githubFetch.c_str(), 0, &jerr);
        if (const int ret = json_unpack(jshit, "{s:s}", "tag_name", updateNumber); ret != 0 || updateNumber == nullptr)
        {
            brls::Logger::warning("Failed to get version info.");
            brls::Application::notify("Failed to get version info!");
        }else
        {
            brls::Application::notify(updateNumber);
            
        }
        json_decref(jshit);
    }

    // Run the app
    while (brls::Application::mainLoop())
        ;

    // Exit
    return EXIT_SUCCESS;
}
