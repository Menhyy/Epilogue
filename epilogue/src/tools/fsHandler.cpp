//
// Created by cher on 08/08/2026.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include "fsHandler.h"
#include <borealis.hpp>
#include <filesystem>
#include <minizip/unzip.h>

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
    if (fs::path parsedPath = savePath; !parsedPath.parent_path().empty())
        fs::create_directories(parsedPath.parent_path());

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

//I WOULD RATHER DIE THAN MESS WITH ZIP FILES AGAIN.
bool unZipFile(const std::string& zipFile, const std::string& destPath)
{
    unzFile zFile = unzOpen(zipFile.c_str());
    if (!zFile)
    {
        brls::Logger::error("Failed to open {}!", zipFile); return false;
    }
    unz_global_info glob_info;
    if (unzGetGlobalInfo(zFile, &glob_info) != UNZ_OK)
    {
        brls::Logger::error("Failed to read global info from {}!", zipFile); unzClose(zFile); return false;
    }
    if (!fs::is_directory(destPath)) fs::create_directories(destPath);

    for (ulong i = 0; i < glob_info.number_entry; i++)
    {
        char fName[256]; unz_file_info fInfo;
        if (unzGetCurrentFileInfo(zFile, &fInfo, fName, sizeof(fName), nullptr, 0, nullptr, 0))
        {
            brls::Logger::error("Failed to read ZIP Inner File! File: {}", zipFile); unzClose(zFile); return false;
        }
        if (std::string fPath = destPath + "/" + fName; fPath.back() == '/')
        {
            fs::create_directory(fPath); brls::Logger::info("Creating folder {} for zip.", fPath);
        }
        else
        {
            if (fs::path parsePath = fPath; !parsePath.parent_path().empty()) fs::create_directories(parsePath.parent_path());

            if (unzOpenCurrentFile(zFile) != UNZ_OK) {
                brls::Logger::error("Failed to read ZIP Inner File! File: {}", zipFile); unzClose(zFile); return false;
            } std::ofstream OutFile(fPath);
            if (!OutFile.is_open())
            {
                brls::Logger::error("Failed to create output file! File: {}", fPath);
                unzCloseCurrentFile(zFile); unzClose(zFile); return false;
            }
            std::vector<char> buffer(8192); int read = 0;
            do
            {
                read = unzReadCurrentFile(zFile, buffer.data(), buffer.size());
                if (read < 0)
                {
                    brls::Logger::error("We are SOMEHOW writing stuff backwards!!!! File: {}", fPath);
                    OutFile.close(); unzCloseCurrentFile(zFile); unzClose(zFile); return false;
                }
                if (read > 0) OutFile << buffer.data();
            } while (read > 0);
            OutFile.close(); unzCloseCurrentFile(zFile);
        }
        if (i + 1 < glob_info.number_entry)
        {
            if (unzGoToNextFile(zFile) != UNZ_OK) {
                brls::Logger::error("Failed to read next file in ZIP!", zipFile); unzClose(zFile); return false;
            }
        }
    }
    unzClose(zFile);
    return true;
}

void deleteFolderRecursive(const std::string& srcPath)
{
    for (auto& i: fs::directory_iterator(srcPath))
    {
        if (const fs::path& iPath = i.path(); fs::is_directory(iPath)) deleteFolderRecursive(iPath);
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