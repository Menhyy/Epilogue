//
// Created by cher on 09/08/2026.
//

#include <borealis.hpp>
#include "configHandler.h"
#include <jansson.h>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

//static std::string epilogue_config_path = "sdmc:/switch/Epilogue/";
static std::string epilogue_config_path = "switch/Epilogue/"; //For tests.
static std::string epilogue_config_file = "settings.json";
static std::string default_file_path = "romfs:/settings_template.json";
static bool fallback = false;
static json_error_t jerr;

bool startHandler()
{
    if (fs::path epiPath = epilogue_config_path; !fs::is_directory(epiPath))
    {
        try { fs::create_directory(epiPath); }
        catch (const std::runtime_error& e)
        {
            std::string err = e.what();
            brls::Logger::error("Config Folder Creation Failed!!!" + err);
            return false;
        }
    }
    if (!fs::exists(epilogue_config_path + epilogue_config_file))
    {
        try
        {
            std::string res;
            std::stringstream buff;
            std::ofstream newFile(epilogue_config_path + epilogue_config_file);
            std::ifstream tempFile(default_file_path);
            buff << tempFile.rdbuf(); res = buff.str();
            newFile << res; newFile.close(); tempFile.close();
        }
        catch (const std::runtime_error& e)
        {
            std::string err = e.what();
            brls::Logger::error("Config File Creation Failed!!! " + err);
            return false;
        }
    }else{brls::Logger::info("Config File Exists!");}
    brls::Logger::info("Config startup success!!!");
    return true;
}

std::string getConfig(const std::string& optName, const std::string& optType)
{
    std::string res;
    std::ifstream tempFile(epilogue_config_path + epilogue_config_file);
    std::stringstream buff;
    buff << tempFile.rdbuf(); res = buff.str();
    json_t *source = json_loads(res.c_str(), 0, &jerr);
    tempFile.close();
    if (!source)
    {
        brls::Logger::error("Error parsing config JSON.");
        brls::Logger::error(jerr.text);
        brls::Application::notify("Error parsing Config JSON! Please check Logs!");
        return "JERR";
    }
    std::string format = "{s:" + optType + "}";

    if (optType == "s")
    {
        char *parsed = nullptr;
        if (int ret = json_unpack(source, "{s:s}", optName.c_str(), &parsed); ret != 0 || parsed == nullptr)
        {
            brls::Logger::error("Failed to unpack string config value.");
            json_decref(source);
            return "JERR";
        }
        std::string result(parsed);
        json_decref(source); return result;
    }
    int parsed = 0;
    if (int ret = json_unpack(source, format.c_str(), optName.c_str(), &parsed); ret != 0)
    {
        brls::Logger::error("Failed to unpack integer config value.");
        json_decref(source);
        return "JERR";
    }
    std::string result = std::to_string(parsed);
    json_decref(source); return result;
}

bool toggleBoolConfig(const std::string& optName)
{
    std::string res = getConfig(optName, "b");
    bool val = (res == "1");
    std::string jso;
    std::ifstream tempFile(epilogue_config_path + epilogue_config_file);
    std::stringstream buff;
    buff << tempFile.rdbuf(); jso = buff.str();
    tempFile.close();
    json_t *source = json_loads(jso.c_str(), 0, &jerr);
    json_object_set_new(source, optName.c_str(), json_boolean(!val));
    std::ofstream newFile(epilogue_config_path + epilogue_config_file);
    newFile << json_dumps(source, JSON_INDENT(4));
    newFile.close();
    json_decref(source);
    return true;
}

bool setStringConfig(const std::string& optName, const std::string& target)
{
    if (std::string res = getConfig(optName, "s"); res == "JERR")
    {
        brls::Logger::error("Failed to get config!!");
        brls::Application::notify("Error parsing Config JSON! Please check Logs!");
        return false;
    }
    std::string jso;  std::ifstream tempFile(epilogue_config_path + epilogue_config_file);
    std::stringstream buff; buff << tempFile.rdbuf(); jso = buff.str(); tempFile.close();
    json_t *source = json_loads(jso.c_str(), 0, &jerr);
    json_object_set_new(source, optName.c_str(), json_string(target.c_str()));
    std::ofstream newFile(epilogue_config_path + epilogue_config_file);
    newFile << json_dumps(source, JSON_INDENT(4));
    newFile.close();
    json_decref(source);
    return true;
}

std::string getConfigAll()
{
    std::string res;
    std::ifstream tempFile(epilogue_config_path + epilogue_config_file);
    std::stringstream buff; buff << tempFile.rdbuf();
    res = buff.str(); tempFile.close();
    return res;
}