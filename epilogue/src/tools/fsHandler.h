#pragma once
bool writeFile(const std::string& contents, const std::string& savePath);
std::string readTextFile(const std::string& filePath);
std::string stringReplace(std::string contents, const std::string& replaced, const std::string& replacement);
