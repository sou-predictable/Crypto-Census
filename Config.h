#ifndef CRYPTOCENSUS_CONFIG_H
#define CRYPTOCENSUS_CONFIG_H

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

class Config {
    public:
        Config();
        std::string getConfig(std::string configKey, std::string defaultValue);

        int getIntConfig(std::string configKey, int defaultValue);
        int getIntConfig(std::string configKey, int defaultValue, int min, int max);

        long getLongConfig(std::string configKey, long defaultValue);
        long getLongConfig(std::string configKey, long defaultValue, int min, int max);
        
    private:
        std::unordered_map<std::string, std::string> configMap;
};

#endif