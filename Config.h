#ifndef CRYPTOCENSUS_CONFIG_H
#define CRYPTOCENSUS_CONFIG_H

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

class Config {
    public:
        /**
         * Default constructor.
         * 
         * Reads from "config.txt", and creates a map of key-value pairs from the text file.
         * Each key-value pair is represented by a line in the text file, and keys and values are 
         * deliniated by the '=' character.
         */
        Config();

        /**
         * Gets a string configuration.
         * 
         * @param configKey the name of the configuration key to search for.
         * @param defaultValue the value to return by default.
         * @return the configuration value if it is valid; defaultValue otherwise.
         */
        std::string getConfig(std::string configKey, std::string defaultValue);

        /**
         * Gets a int configuration.
         * 
         * @param configKey the name of the configuration key to search for.
         * @param defaultValue the value to return by default.
         * @return the configuration value if it is valid; defaultValue otherwise.
         */
        int getIntConfig(std::string configKey, int defaultValue);

        /**
         * Gets a int configuration if it falls within a specified minimum and maximum value.
         * 
         * @param configKey the name of the configuration key to search for.
         * @param defaultValue the value to return by default.
         * @param min the minimum number to accept.
         * @param max the maximum number to accept.
         * @return the configuration value if it is valid, and is between the min and max values; defaultValue otherwise.
         */
        int getIntConfig(std::string configKey, int defaultValue, int min, int max);

        /**
         * Gets a long configuration.
         * 
         * @param configKey the name of the configuration key to search for.
         * @param defaultValue the value to return by default.
         * @return the configuration value if it is valid; defaultValue otherwise.
         */
        long getLongConfig(std::string configKey, long defaultValue);

        /**
         * Gets a long configuration if it falls within a specified minimum and maximum value.
         * 
         * @param configKey the name of the configuration key to search for.
         * @param defaultValue the value to return by default.
         * @param min the minimum number to accept.
         * @param max the maximum number to accept.
         * @return the configuration value if it is valid, and is between the min and max values; defaultValue otherwise.
         */
        long getLongConfig(std::string configKey, long defaultValue, int min, int max);
        
    private:
        std::unordered_map<std::string, std::string> configMap;
};

#endif