//
// Created by cher on 08/08/2026.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include "fsHandler.h"
#include <borealis.hpp>
#include <filesystem>

namespace fs = std::filesystem;

std::string stringReplace(std::string contents, const std::string& replaced, const std::string& replacement)
{
    const size_t spos = contents.find(replaced);
    if (spos == std::string::npos) return "";
    return contents.replace(spos, replaced.length(), replacement);
}

bool writeTextFile(const std::string& contents, const std::string& savePath)
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

bool copyFile(const std::string& srcPath, const std::string& savePath)
{
    std::ifstream srcFile(srcPath);
    std::ofstream newFile(savePath);
    if (!newFile || !srcFile)
    {
        brls::Logger::error("Failed to copy file " + srcPath + " to " + savePath + "!");
        brls::Application::notify("Failed to copy file!\nPlease check logs for more info.");
        return false;
    }
    newFile << srcFile.rdbuf();
    newFile.close();
    srcFile.close();
    brls::Logger::info("Copied file " + srcPath + " to " + savePath + "!");
    return true;
}

void copyFolderRecursive(const std::string& srcPath, const std::string& destPath)
{
    fs::create_directories(destPath);
    for (auto& i: fs::directory_iterator(srcPath))
    {
        const fs::path& iPath = i.path();
        const fs::path& dPath = destPath / iPath.filename();

        if (fs::is_directory(iPath)) copyFolderRecursive(iPath, dPath);
        else copyFile(iPath, dPath);
    }
}

void deleteFolderRecursive(const std::string& srcPath)
{
    for (auto& i: fs::directory_iterator(srcPath))
    {
        const fs::path& iPath = i.path();

        if (fs::is_directory(iPath)) deleteFolderRecursive(iPath);
        else fs::remove(iPath);
    }
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