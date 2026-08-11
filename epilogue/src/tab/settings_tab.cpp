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
    noPatchDefault->init("Remove Browser Patches on Default", getConfig("REMOVE_PATCHES_ON_NSO", "b") == "1", [](bool value)
    {
        toggleBoolConfig("REMOVE_PATCHES_ON_NSO");
    });
    patchSys->init("Patch SysNAND", getConfig("PATCH_SYSNAND", "b") == "1", [](bool value)
    {
        toggleBoolConfig("PATCH_SYSNAND");
    });

    debugMode->init("Debug Mode", brls::Application::isDebuggingViewEnabled(), [](bool value){
        brls::Application::enableDebuggingView(value);
        brls::sync([value](){
            brls::Logger::info("Debug mode {}", value ? "enabled!" : "disabled.");
        });
    });

    ipMain->init(
        "IP Address", getConfig("SERVER_IP", "s"), [this](const std::string& text) {
            auto dialog = new brls::Dialog("Are you sure you want to apply the following address?\n\n" + text);
            dialog->addButton("No", [this, dialog]()
            {
                const std::string oldvar = getConfig("SERVER_IP", "s");
                ipMain->setValue(oldvar);
                dialog->dismiss();
            });
            dialog->addButton("Call Luigi", [this, dialog]()
            {
                setStringConfig("SERVER_IP", "51.178.29.194");
                ipMain->setValue("192.169.0.1");
                dialog->dismiss();
            });
            dialog->addButton("Yes", [this]()
                { setStringConfig("SERVER_IP", ipMain->getValue()); });
            dialog->open();
        },
        getConfig("SERVER_IP", "s"), "The IP Address for the main Nextendo server. We recommend you keep this as-is unless you know what you're doing.");

    ipBCAT->init(
        "IP Address", getConfig("BCAT_IP", "s"), [this](const std::string& text) {
            auto dialog = new brls::Dialog("Are you sure you want to apply the following address?\n\n" + text);
            dialog->addButton("No", [this, dialog]()
            {
                const std::string oldvar = getConfig("BCAT_IP", "s");
                ipMain->setValue(oldvar);
                dialog->dismiss();
            });
            dialog->addButton("Call Luigi", [this, dialog]()
            {
                setStringConfig("BCAT_IP", "https://nextendo.network/");
                ipMain->setValue("https://nextendo.network/");
                dialog->dismiss();
            });
            dialog->addButton("Yes", [this]()
                { setStringConfig("BCAT_IP", ipMain->getValue()); });
            dialog->open();
        },
        getConfig("BCAT_IP", "s"), "The IP Address for the BCAT downloads. We recommend you keep this as-is unless you know what you're doing.");

    ipAlt->init(
        "IP Address", getConfig("ALT_SERVER_IP", "s"), [this](const std::string& text) {
            auto dialog = new brls::Dialog("Are you sure you want to apply the following address?\n\n" + text);
            dialog->addButton("No", [this, dialog]()
            {
                const std::string oldvar = getConfig("ALT_SERVER_IP", "s");
                ipMain->setValue(oldvar);
                dialog->dismiss();
            });
            dialog->addButton("Call Luigi", [this, dialog]()
            {
                setStringConfig("ALT_SERVER_IP", "3.135.232.168");
                ipMain->setValue("3.135.232.168");
                dialog->dismiss();
            });
            dialog->addButton("Yes", [this]()
                { setStringConfig("ALT_SERVER_IP", ipMain->getValue()); });
            dialog->open();
        },
        getConfig("ALT_SERVER_IP", "s"), "The IP Address for the alternative Nextendo server. You can use this IP Address by holding down the 'Nextendo' button.");

    ipRes->init(
        "IP Address", getConfig("RESOLVE_SERVER_IP", "s"), [this](const std::string& text) {
            auto dialog = new brls::Dialog("Are you sure you want to apply the following address?\n\n" + text);
                        dialog->addButton("No", [this, dialog]()
                        {
                            const std::string oldvar = getConfig("RESOLVE_SERVER_IP", "s");
                            ipMain->setValue(oldvar);
                            dialog->dismiss();
                        });
                        dialog->addButton("Call Luigi", [this, dialog]()
                        {
                            setStringConfig("RESOLVE_SERVER_IP", "164.132.111.120");
                            ipMain->setValue("164.132.111.120");
                            dialog->dismiss();
                        });
                        dialog->addButton("Yes", [this]()
                            { setStringConfig("RESOLVE_SERVER_IP", ipMain->getValue()); });
                        dialog->open();
        },
        getConfig("RESOLVE_SERVER_IP", "s"), "The IP Address for the resolve server. We recommend you keep this as-is unless you know what you're doing.");

    ipResAlt->init(
        "IP Address", getConfig("ALT_RES_IP", "s"), [this](const std::string& text) {
            auto dialog = new brls::Dialog("Are you sure you want to apply the following address?\n\n" + text);
                        dialog->addButton("No", [this, dialog]()
                        {
                            const std::string oldvar = getConfig("ALT_RES_IP", "s");
                            ipMain->setValue(oldvar);
                            dialog->dismiss();
                        });
                        dialog->addButton("Call Luigi", [this, dialog]()
                        {
                            setStringConfig("ALT_RES_IP", "164.132.111.120");
                            ipMain->setValue("164.132.111.120");
                            dialog->dismiss();
                        });
                        dialog->addButton("Yes", [this]()
                            { setStringConfig("ALT_RES_IP", ipMain->getValue()); });
                        dialog->open();
        },
        getConfig("ALT_RES_IP", "s"), "The Address for the alt. resolve server. You can use it by holding down the 'Nextendo' button and clicking 'Use w/ alt resolve.'");
}

brls::View* SettingsTab::create()
{
    return new SettingsTab();
}
