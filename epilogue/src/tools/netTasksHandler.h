#pragma once
#include <string>
bool writeFileNet(const std::string& URL, const std::string& filePath, const std::string& savePath);
std::string readFileNet(const std::string& URL, const std::string& filePath);