#include "tab/epilogue_main.hpp"

#include <filesystem>
#include <iostream>

#include "../tools/fsHandler.h"
#include "../tools/netTasksHandler.h"
#include "src/tools/configHandler.h"

namespace fs = std::filesystem;

EpilogueMain::EpilogueMain()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/epilogue.xml");

    // Bind the button click to a method using the macro (just for the sake of showcasing it, it's overkill in this situation)
    BRLS_REGISTER_CLICK_BY_ID("button_primary", this->onNextendoButtonClicked);
    BRLS_REGISTER_CLICK_BY_ID("button_net", this->onDefaultButtonClicked);
    BRLS_REGISTER_CLICK_BY_ID("button_placeholder", this->onPlaceholderButtonClicked);
}

static bool isThisNextendo(const std::string& contents)
{
    if (contents.find("#epiwashere") != std::string::npos) return true;
    if (contents.find("NEXTENDO NETWORK - Atmosphere DNS-MITM") != std::string::npos) return true;
    if (contents.find(getConfig("SERVER_IP", "s")) != std::string::npos) return true;
    return false;
}

static void backupHosts(const bool sys)
{
    if (!fs::exists("sdmc:/switch/Epilogue/backup"))
    {
        brls::Logger::warning("Backups folder not found, creating one...");
        fs::create_directory("sdmc:/switch/Epilogue/backup");
    }
    if (sys)
    {
        if (!fs::exists("sdmc:/atmosphere/hosts/sysmmc.txt")) brls::Logger::info("Nothing to back up.");
        else if (isThisNextendo(readTextFile("sdmc:/atmosphere/hosts/sysmmc.txt")))
        {
            brls::Logger::info("This is ours! Nothing to back up.");
        }else
        {
            if (const std::string sysBak = readTextFile("sdmc:/atmosphere/hosts/sysmmc.txt");
            !writeTextFile(sysBak, "sdmc:/switch/Epilogue/backup/sysmmc.bak"))
            {
                brls::Logger::error("Failed to back up SysNAND hosts file!");
                brls::Application::notify("Failed to back up SysNAND hosts.\nPlease check logs!");
            }else if (isThisNextendo(readTextFile("sdmc:/atmosphere/hosts/sysmmc.txt")))
            {
                brls::Logger::info("This is ours! Nothing to back up.");
            }
            {
                brls::Logger::info("Backed up SysNAND hosts file.");
                brls::Application::notify("Backed up SysNAND hosts file.");
            }
        }
    }
    if (!fs::exists("sdmc:/atmosphere/hosts/emummc.txt")) brls::Logger::info("Nothing to back up.");
    else if (isThisNextendo(readTextFile("sdmc:/atmosphere/hosts/emummc.txt")))
    {
        brls::Logger::info("This is ours! Nothing to back up.");
    }else
    {
        if (const std::string emuBak = readTextFile("sdmc:/atmosphere/hosts/emummc.txt");
    !writeTextFile(emuBak, "sdmc:/switch/Epilogue/backup/emummc.bak"))
        {
            brls::Logger::error("Failed to back up EmuNAND hosts file!");
            brls::Application::notify("Failed to back up EmuNAND hosts.\nPlease check logs!");
        }else
        {
            brls::Logger::info("Backed up EmuNAND hosts file.");
            brls::Application::notify("Backed up EmuNAND hosts file.");
        }
    }
}

static bool patchSystem(const std::string& hostsContent, const bool patchSys)
{
    if (patchSys)
    {
        if (!writeTextFile(hostsContent, "sdmc:/atmosphere/hosts/sysmmc.txt"))
        {
            brls::Logger::error("Failed to write SysMMC hosts!!!!");
            fs::remove("sdmc:/atmosphere/hosts/sysmmc.txt");
            if (fs::exists("sdmc:/switch/Epilogue/backup/sysmmc.bak"))
            {
                writeTextFile(readTextFile("sdmc:/switch/Epilogue/backup/sysmmc.bak"),
                    "sdmc:/atmosphere/hosts/sysmmc.txt");
                brls::Logger::warning("We had to restore a backup!");
            }
            const auto dialog = new brls::Dialog("Failed to apply Nextendo SysMMC hosts file.\nThe previous host file was restored, and if there wasn't a previous, all host files were removed.");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }
    if (!writeTextFile(hostsContent, "sdmc:/atmosphere/hosts/emummc.txt"))
    {
        brls::Logger::error("Failed to write EmuMMC hosts!!!!");
        fs::remove("sdmc:/atmosphere/hosts/emummc.txt");
        if (fs::exists("sdmc:/switch/Epilogue/backup/emummc.bak"))
        {
            writeTextFile(readTextFile("sdmc:/switch/Epilogue/backup/emummc.bak"),
                "sdmc:/atmosphere/hosts/emummc.txt");
            brls::Logger::warning("We had to restore a backup!");
        }
        const auto dialog = new brls::Dialog("Failed to apply Nextendo EmuMMC hosts file.\nThe previous host file was restored, and if there wasn't a previous, all host files were removed.");
        dialog->addButton("Ok", [](){}); dialog->open(); return false;
    }
    if (!fs::exists("sdmc:/exosphere.ini"))
    {
        if (!writeTextFile("[exosphere]\nblank_prodinfo_emummc=0", "sdmc:/exosphere.ini"))
        {
            brls::Logger::error("Could not make new exosphere.ini file.");
            const auto dialog = new brls::Dialog("Failed to create exosphere file! ProdInfo will be untouched and possibly won't be blanked on EmuNAND!!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }

    }else
    {
        const std::string newStringE = stringReplace(readTextFile("sdmc:/exosphere.ini"),
            "blank_prodinfo_emummc=1",
            "blank_prodinfo_emummc=0");
        if (!writeTextFile(newStringE ,"sdmc:/exosphere.ini"))
        {
            brls::Logger::error("Could not edit exosphere.ini file.");
            const auto dialog = new brls::Dialog("Failed to edit exosphere file! ProdInfo possibly won't be blanked on EmuNAND!!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }
    if (!fs::exists("sdmc:/atmosphere/config/system_settings.ini"))
    {
        if (!writeTextFile("[atmosphere]\nenable_dns_mitm = u8!0x1\nadd_defaults_to_dns_hosts = u8!0x1", "sdmc:/atmosphere/config/system_settings.ini"))
        {
            brls::Logger::error("Could not make new system_settings file.");
            const auto dialog = new brls::Dialog("Failed to create atmosphere settings file! Host files will not work properly!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }

    }else
    {
        std::string newStringA = stringReplace(readTextFile("sdmc:/atmosphere/config/system_settings.ini"),
            "enable_dns_mitm = u8!0x0",
            "enable_dns_mitm = u8!0x1");
        newStringA = stringReplace(newStringA, "add_defaults_to_dns_hosts = u8!0x0",
            "add_defaults_to_dns_hosts = u8!0x1");
        if (!writeTextFile(newStringA ,"sdmc:/atmosphere/config/system_settings.ini"))
        {
            brls::Logger::error("Could not edit system_settings.ini file.");
            const auto dialog = new brls::Dialog("Failed to edit atmosphere settings file! Hosts file probably won't work properly!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }

    brls::Application::notify("Patched!");
    brls::Logger::info("Patching successful.");
    return true;
}

// Apply Nextendo patches.
static bool applyNextendo()
{
    std::string hostsBoilContent;
    bool patchSys = (getConfig("PATCH_SYSNAND", "b") == "1");

    // Check, find and read the hosts file boilerplate;
    // Check filesystem first, as it'll have the hosts file downloaded from the internet.
    // If it doesn't exist, use the RomFS template, which is bundled with Epilogue but may not be updated.
    if (fs::exists("sdmc:/switch/Epilogue/nxhosts_local.txt"))
    {
        if ((hostsBoilContent = readTextFile("sdmc:/switch/Epilogue/nxhosts_local.txt")) == "ERR")
            brls::Logger::error("Could not read local hosts file!!!");

        hostsBoilContent = readTextFile("romfs:/nextendo/nxhosts_bundled.txt");
        writeTextFile(hostsBoilContent, "sdmc:/switch/Epilogue/nxhosts_local.txt");
    }else
    {
        hostsBoilContent = readTextFile("romfs:/nextendo/nxhosts_bundled.txt");
        if (hostsBoilContent == "ERR")
        {
            brls::Application::notify(hostsBoilContent);
            brls::Logger::error("Could not read romfs hosts file!!!");
            const auto dialog = new brls::Dialog("Failed to apply Nextendo hosts file.");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }

    // Replace dummy strings in hosts file with their proper values from the config file.
    const std::string hostsContent = fmt::format(fmt::runtime(hostsBoilContent),
        fmt::arg("SERVER", getConfig("SERVER_IP", "s")),
        fmt::arg("RESOLVE", getConfig("RESOLVE_SERVER_IP", "s"))
        );

    //Check for browser patches, if don't exist, apply.
    if (!fs::is_directory("sdmc:/atmosphere/exefs_patches/disable_ca_verification"))
    {
        brls::Logger::info("Applying browser patches...");
        try
        {
            copyFolderRecursive("romfs:/nextendo/patches/atmosphere", "sdmc:/atmosphere/");
        }catch (const fs::filesystem_error& e)
        {
            brls::Logger::error("Failed to copy browser patches: " + std::string(e.what()));
            const auto dialog = new brls::Dialog("Failed to apply Browser Patches.\n" + std::string(e.what()));
            dialog->addButton("Ok", [](){});
            dialog->open();
            return false;
        }
    }else
    {
        brls::Logger::info("NOT applying browser patches...");
    }

    if (getConfig("AUTO_BACKUP_HOSTS", "b") != "1" && (getConfig("SHUT_UP_BACKUPS", "b") != "1"))
    {
        const auto dialog = new brls::Dialog("Do you want to back up your current hosts file?");
        dialog->addButton("Yes", [patchSys, hostsContent]()
        {
            backupHosts(patchSys);
            patchSystem(hostsContent, patchSys);
        });
        dialog->addButton("No", [hostsContent, patchSys]()
        {
            patchSystem(hostsContent, patchSys);
        });
        dialog->addButton("No, stop asking.", [patchSys, hostsContent]()
        {
            toggleBoolConfig("SHUT_UP_BACKUPS");
            patchSystem(hostsContent, patchSys);
        });
        dialog->open();
    }else if (getConfig("AUTO_BACKUP_HOSTS", "b") == "1")
    {
        backupHosts(patchSys);
        patchSystem(hostsContent, patchSys);
    }else
    {
        patchSystem(hostsContent, patchSys);
    }
    return true;
}

static bool applyS2Bcat()
{
    writeFileNet(getConfig("BCAT_IP", "s"), "/api/bcat/01003BC0000A0000", "switch/Epilogue/test.zip");
    unZipFile("switch/Epilogue/test.zip", "switch/Epilogue/NetDecompress");
    brls::Logger::info("Hello!");
    return true;
}

// Remove Nextendo patches and go back to default.
static bool applyDefaults()
{
    bool emuBackup = false;
    bool sysBackup = false;
    const bool patchSys = (getConfig("PATCH_SYSNAND", "b") == "1");
    const bool deletePatches = (getConfig("REMOVE_PATCHES_ON_NSO", "b") == "1");
    if (!fs::exists("sdmc:/switch/Epilogue/backup"))
    {
        brls::Logger::warning("Backups folder not found, creating one but proceeding without restore;");
        fs::create_directory("sdmc:/switch/Epilogue/backup");
    }else
    {
        if (fs::exists("sdmc:/switch/Epilogue/backup/emummc.bak"))
        {
            emuBackup = true;
            brls::Logger::info("Found emuNAND backup.");
        }
        if (fs::exists("sdmc:/switch/Epilogue/backup/sysmmc.bak"))
        {
            sysBackup = true;
            brls::Logger::info("Found sysNAND backup.");
        }
    }

    if (emuBackup)
    {
        writeTextFile(readTextFile("sdmc:/switch/Epilogue/backup/emummc.bak"),
                "sdmc:/atmosphere/hosts/emummc.txt");
        brls::Logger::warning("EmuMMC backup restored!");
        brls::Application::notify("Restored EmuNAND hosts file backup.");
    }else
    {
        fs::remove("sdmc:/atmosphere/hosts/emummc.txt");
        brls::Logger::warning("EmuMMC hosts file deleted!");
        brls::Application::notify("Deleted EmuNAND hosts file.");
    }

    if (sysBackup && !patchSys)
    {
        writeTextFile(readTextFile("sdmc:/switch/Epilogue/backup/sysmmc.bak"),
                "sdmc:/atmosphere/hosts/sysmmc.txt");
        brls::Logger::warning("SysMMC backup restored!");
        brls::Application::notify("Restored SysNAND hosts file backup.");
    }else if (!patchSys)
    {
        fs::remove_all("sdmc:/atmosphere/hosts/sysmmc.txt");
        brls::Logger::warning("SysMMC hosts file deleted!");
        brls::Application::notify("Deleted SysNAND hosts file.");
    }

    if (deletePatches)
    {
        try{ fs::remove_all("sdmc:/atmosphere/contents/0100000000000803"); }
        catch (const fs::filesystem_error& e)
        {
            brls::Logger::error("Failed to delete browser patches: " + std::string(e.what()));
            const auto dialog = new brls::Dialog("Failed to remove Browser Patches. Please remove them manually if possible.");
            dialog->addButton("Ok", [](){}); dialog->open();
        }
        try{ fs::remove_all("sdmc:/atmosphere/exefs_patches/disable_ca_verification"); }
        catch (const fs::filesystem_error& e)
        {
            brls::Logger::error("Failed to delete browser patches: " + std::string(e.what()));
            const auto dialog = new brls::Dialog("Failed to remove Browser Patches. Please remove them manually if possible.");
            dialog->addButton("Ok", [](){}); dialog->open();
        }
        try{ fs::remove_all("sdmc:/atmosphere/nro_patches/disable_browser_ca_verification"); }
        catch (const fs::filesystem_error& e)
        {
            brls::Logger::error("Failed to delete browser patches: " + std::string(e.what()));
            const auto dialog = new brls::Dialog("Failed to remove Browser Patches. Please remove them manually if possible.");
            dialog->addButton("Ok", [](){}); dialog->open();
        }
    }

    if (!fs::exists("sdmc:/exosphere.ini"))
    {
        if (!writeTextFile("[exosphere]\nblank_prodinfo_emummc=0", "sdmc:/exosphere.ini"))
        {
            brls::Logger::error("Could not make new exosphere.ini file.");
            const auto dialog = new brls::Dialog("Failed to create exosphere file! ProdInfo will be untouched and possibly won't be blanked on EmuNAND!!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }

    }else
    {
        const std::string newStringE = stringReplace(readTextFile("sdmc:/exosphere.ini"),
            "blank_prodinfo_emummc=1",
            "blank_prodinfo_emummc=0");
        if (!writeTextFile(newStringE ,"sdmc:/exosphere.ini"))
        {
            brls::Logger::error("Could not edit exosphere.ini file.");
            const auto dialog = new brls::Dialog("Failed to edit exosphere file! ProdInfo possibly won't be blanked on EmuNAND!!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }
    if (!fs::exists("sdmc:/atmosphere/config/system_settings.ini"))
    {
        if (!writeTextFile("[atmosphere]\nenable_dns_mitm = u8!0x1\nadd_defaults_to_dns_hosts = u8!0x0", "sdmc:/atmosphere/config/system_settings.ini"))
        {
            brls::Logger::error("Could not make new system_settings file.");
            const auto dialog = new brls::Dialog("Failed to create atmosphere settings file! Host files will not work properly!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }else
    {
        const std::string newStringA = stringReplace(readTextFile("sdmc:/atmosphere/config/system_settings.ini"), "add_defaults_to_dns_hosts = u8!0x0",
            "add_defaults_to_dns_hosts = u8!0x1");
        if (!writeTextFile(newStringA ,"sdmc:/atmosphere/config/system_settings.ini"))
        {
            brls::Logger::error("Could not edit system_settings.ini file.");
            const auto dialog = new brls::Dialog("Failed to edit atmosphere settings file! Hosts file probably won't work properly!");
            dialog->addButton("Ok", [](){}); dialog->open(); return false;
        }
    }

    brls::Application::notify("Success!!");
    return true;
}

static void spawnConfirmPoupup()
{
    const auto confirmDiag = new brls::Dialog("Do you want to apply Nextendo patches to your system?");
    confirmDiag->addButton("Yes", []()
    {
        brls::Application::notify("Patching!");
        applyNextendo();
    });
    confirmDiag->addButton("Yes, don't ask again.", []()
    {
        toggleBoolConfig("SHUT_UP_WARNINGS");
        brls::Application::notify("Patching!");
        applyNextendo();
    });
    confirmDiag->addButton("No", []()
    {
        brls::Application::notify("Cancelled!");
        return false;
    });
    confirmDiag->open();
}

bool EpilogueMain::onDefaultButtonClicked(brls::View* view)
{
    if (getConfig("SHUT_UP_WARNINGS", "b") == "0")
    {
        const auto confirmDiag = new brls::Dialog("Do you want to revert the Nextendo patches?");
        confirmDiag->addButton("Yes", []()
        {
            brls::Application::notify("Removing!");
            applyDefaults();
        });
        confirmDiag->addButton("Yes, don't ask again.", []()
        {
            toggleBoolConfig("SHUT_UP_WARNINGS");
            brls::Application::notify("Removing!");
            applyDefaults();
        });
        confirmDiag->addButton("No", []()
        {
            brls::Application::notify("Cancelled!");
            return false;
        });
        confirmDiag->open();
    }else
    {
        brls::Application::notify("Removing!");
        applyDefaults();
    }

    return true;
}

bool EpilogueMain::onNextendoButtonClicked(brls::View* view)
{
    if (getConfig("SHUT_UP_WARNINGS", "b") == "0")
    {
        const bool syse = fs::exists("sdmc:/atmosphere/hosts/sysmmc.txt");
        const bool emue = fs::exists("sdmc:/atmosphere/hosts/emummc.txt");
        bool doWeHaveNextendoAlready = false;

        if (syse){ if (const std::string sysm = readTextFile("sdmc:/atmosphere/hosts/sysmmc.txt"); isThisNextendo(sysm)) doWeHaveNextendoAlready = true; }
        else if (emue) { if (const std::string emum = readTextFile("sdmc:/atmosphere/hosts/emummc.txt"); isThisNextendo(emum)) doWeHaveNextendoAlready = true; }

        if (doWeHaveNextendoAlready)
        {
            const auto nextendoDiag = new brls::Dialog("Your hosts file is already patched with Nextendo.\n"
                                                      "Do you still want to proceed?");
            nextendoDiag->addButton("Yes", []()
            {
                spawnConfirmPoupup();
                return true;
            });
            nextendoDiag->addButton("No", []()
            {
                brls::Application::notify("Cancelled!");
                return false;
            });
            nextendoDiag->open();
        }else
        {
            spawnConfirmPoupup();
        }
    }else
    {
        brls::Application::notify("Patching!");
        applyNextendo();
    }
    return true;
}

bool EpilogueMain::onPlaceholderButtonClicked(brls::View* view)
{
    applyS2Bcat();
    return true;
}

brls::View* EpilogueMain::create()
{
    return new EpilogueMain();
}
