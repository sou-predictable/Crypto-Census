#include "Config.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

Config::Config() {
    const std::string configFileName = "config.txt";
    std::ifstream configFileStream(configFileName);
    std::string configKey;
    std::string configValue;

    while(std::getline(configFileStream, configKey, '=') && std::getline(configFileStream, configValue)) {
        configMap.insert(std::make_pair(configKey, configValue));
    }
}

std::string Config::getConfig(std::string configKey, std::string defaultValue) {
    std::unordered_map<std::string, std::string>::iterator it = configMap.find(configKey);
    if(!configMap.empty() && it == configMap.end()) {
        return it->second;
    }
    return defaultValue;
}

int Config::getIntConfig(std::string configKey, int defaultValue) {
    return getIntConfig(configKey, defaultValue, 0, INT_MAX);
}

int Config::getIntConfig(std::string configKey, int defaultValue, int min, int max) {
    std::unordered_map<std::string, std::string>::iterator it = configMap.find(configKey);
    if(!configMap.empty() && it != configMap.end()) {
        try {
            int output = stoi(it->second);
            if(output >= min && output <= max)
                return output;
        } catch(const std::invalid_argument& e) {
            std::cout << "ERROR: Invalid Configuration: " << configKey << "\n";
        } catch(const std::out_of_range& e) {
            std::cout << "ERROR: Configuration Out Of Range: " << configKey << "\n";
        }
    }
    return defaultValue;
}

long Config::getLongConfig(std::string configKey, long defaultValue) {
    return getLongConfig(configKey, defaultValue, 0, LONG_MAX);
}

long Config::getLongConfig(std::string configKey, long defaultValue, int min, int max) {
    std::unordered_map<std::string, std::string>::iterator it = configMap.find(configKey);
    if(!configMap.empty() && it != configMap.end()) {
        try {
            long output = stol(it->second);
            return output;
        } catch(const std::invalid_argument& e) {
            std::cout << "ERROR: Invalid Configuration: " << configKey << "\n";
        } catch(const std::out_of_range& e) {
            std::cout << "ERROR: Configuration Out Of Range: " << configKey << "\n";
        }
    }
    return defaultValue;
}