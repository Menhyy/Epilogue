#pragma once
bool writeTextFile(const std::string& contents, const std::string& savePath);
std::string readTextFile(const std::string& filePath);
std::string stringReplace(std::string contents, const std::string& replaced, const std::string& replacement);
void copyFolderRecursive(const std::string& srcPath, const std::string& destPath);
bool copyFile(const std::string& srcPath, const std::string& savePath);
void deleteFolderRecursive(const std::string& srcPath);
bool unZipFile(const std::string& zipFile, const std::string& destPath);

