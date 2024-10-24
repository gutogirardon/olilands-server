// src/config/configmanager.h
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <luajit-2.1/lua.hpp>

class ConfigManager {
public:
    ConfigManager(const std::string& configPath);
    ~ConfigManager();
    bool loadConfig();

    // Getters
    std::string getLogLevel() const;
    double getMemoryThreshold() const;
    int getServerPort() const;

private:
    std::string configPath_;
    lua_State* L_;

    // Config variables
    std::string log_level_;
    double memory_threshold_;
    int server_port_;
};

#endif // CONFIGMANAGER_H
