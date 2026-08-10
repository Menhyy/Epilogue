//
// Created by cher on 08/08/2026.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include "fsHandler.h"
#include <borealis.hpp>

std::string stringReplace(std::string contents, const std::string& replaced, const std::string& replacement)
{
    const size_t spos = contents.find(replaced);
    if (spos == std::string::npos) return "";
    return contents.replace(spos, replaced.length(), replacement);
}

bool writeFile(const std::string& contents, const std::string& savePath)
{
    std::ofstream newFile(savePath);
    if (!newFile.is_open())
    {
        brls::Logger::error("Failed to write to file " + savePath + "!");
        brls::Application::notify("Failed to open file!\nPlease check logs for more info.");
        return false;
    }
    newFile << contents;
    newFile.close();
    return true;
}

std::string readTextFile(const std::string& filePath)
{
    std::ostringstream buffer;
    std::string contents;
    std::ifstream readFile(filePath);
    if (!readFile.is_open())
    {
        brls::Logger::error("Could not read file " + filePath + "!");
        brls::Application::notify("Failed to open file!\nPlease check logs for more info.");
        return "ERR";
    }
    buffer << readFile.rdbuf();
    contents = buffer.str();
    readFile.close();
    return contents;
}