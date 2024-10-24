// src/config/configmanager.h
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

extern "C" {
#include <luajit-2.1/lua.hpp>
}

#include <string>

class ConfigManager {
public:
    ConfigManager(const std::string& configPath);
    ~ConfigManager();
    bool loadConfig();

    // Getters para as configurações gerais
    std::string getLogLevel() const;
    double getMemoryThreshold() const;
    int getServerPort() const;

    // Getters para as configurações do MySQL
    std::string getMySQLUser() const;
    std::string getMySQLPassword() const;
    std::string getMySQLHost() const;
    int getMySQLPort() const;
    std::string getMySQLDatabase() const;

private:
    std::string configPath_;
    lua_State* L_;

    // Config variables
    std::string log_level_;
    double memory_threshold_;
    int server_port_;

    // MySQL config variables
    std::string mysql_user_;
    std::string mysql_password_;
    std::string mysql_host_;
    int mysql_port_;
    std::string mysql_database_;
};

#endif // CONFIGMANAGER_H
