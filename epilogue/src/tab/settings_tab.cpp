#include "tab/settings_tab.hpp"
#include "../tools/configHandler.h"
#include <jansson.h>

using namespace brls::literals;  // for _i18n

SettingsTab::SettingsTab()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/settings.xml");

    hostBack->init("Auto-Backup Hosts", getConfig("AUTO_BACKUP_HOSTS", "b") == "1", [](bool value)
    {
        toggleBoolConfig("AUTO_BACKUP_HOSTS");
    });
    noPatchDefault->init("Keep Browser Patches on Default", getConfig("REMOVE_PATCHES_ON_NSO", "b") == "1", [](bool value)
    {
        toggleBoolConfig("REMOVE_PATCHES_ON_NSO");
    });
    patchSys->init("Patch SysNAND", getConfig("PATCH_SYSNAND", "b") == "1", [](bool value)
    {
        toggleBoolConfig("PATCH_SYSNAND");
    });

    ipMain->init(
        "IP Address", getConfig("SERVER_IP", "s"), [](const std::string& text) {
            auto dialog = new brls::Dialog("This IP Address is Invalid! Do you want to continue?");
            dialog->addButton("No", []() {});
            dialog->addButton("Call Luigi", []() {});
            dialog->addButton("Yes", []()
                { brls::Application::notify("NAH"); });
            dialog->open();
        },
        getConfig("SERVER_IP", "s"), "The IP Address for the main Nextendo server. We recommend you keep this as-is unless you know what you're doing.");
    ipAlt->init(
        "IP Address", getConfig("ALT_SERVER_IP", "s"), [](const std::string& text) {

        },
        getConfig("ALT_SERVER_IP", "s"), "The IP Address for the alternative Nextendo server. You can use this IP Address by holding down the 'Nextendo' button.");
    ipRes->init(
        "IP Address", getConfig("RESOLVE_SERVER_IP", "s"), [](const std::string& text) {

        },
        getConfig("RESOLVE_SERVER_IP", "s"), "The IP Address for the resolve server. We recommend you keep this as-is unless you know what you're doing.");
    ipResAlt->init(
        "IP Address", getConfig("ALT_RES_IP", "s"), [](const std::string& text) {

        },
        getConfig("ALT_RES_IP", "s"), "The Address for the alt. resolve server. You can use it by holding down the 'Nextendo' button and clicking 'Use w/ alt resolve.'");
}

brls::View* SettingsTab::create()
{
    return new SettingsTab();
}
