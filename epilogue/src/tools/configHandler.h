#pragma once
#include <string>

bool startHandler();
std::string getConfig(const std::string& optName, const std::string& optType);
std::string getConfigAll();
bool toggleBoolConfig(const std::string& optName);
bool setStringConfig(const std::string& optName, const std::string& target);