// src/config/configmanager.cpp
#include "configmanager.h"
#include <spdlog/spdlog.h>

ConfigManager::ConfigManager(const std::string& configPath)
    : configPath_(configPath), L_(luaL_newstate()) {
    luaL_openlibs(L_);
}

ConfigManager::~ConfigManager() {
    if (L_) {
        lua_close(L_);
    }
}

bool ConfigManager::loadConfig() {
    spdlog::info("Loading config.lua....");
    if (luaL_dofile(L_, configPath_.c_str()) != LUA_OK) {
        spdlog::error("Failed to load config.lua: {}", lua_tostring(L_, -1));
        return false;
    }

    lua_getglobal(L_, "config");
    if (!lua_istable(L_, -1)) {
        spdlog::error("Config table not found in config.lua");
        return false;
    }

    // log_level
    lua_getfield(L_, -1, "log_level");
    if (lua_isstring(L_, -1)) {
        log_level_ = lua_tostring(L_, -1);
    } else {
        log_level_ = "info"; // default
        spdlog::warn("log_level not found or not a string. Using default 'info'.");
    }
    lua_pop(L_, 1);

    // memory_threshold
    lua_getfield(L_, -1, "memory_threshold");
    if (lua_isnumber(L_, -1)) {
        memory_threshold_ = lua_tonumber(L_, -1);
    } else {
        memory_threshold_ = 80.0; // default
        spdlog::warn("memory_threshold not found or not a number. Using default 80.0.");
    }
    lua_pop(L_, 1);

    // server_port
    lua_getfield(L_, -1, "server_port");
    if (lua_isnumber(L_, -1)) {
        server_port_ = static_cast<int>(lua_tointeger(L_, -1));
    } else {
        server_port_ = 7272; // default
        spdlog::warn("server_port not found or not a number. Using default 7272.");
    }
    lua_pop(L_, 1);

    // MySQL configurations
    lua_getfield(L_, -1, "mysql");
    if (lua_istable(L_, -1)) {
        lua_getfield(L_, -1, "user");
        mysql_user_ = lua_isstring(L_, -1) ? lua_tostring(L_, -1) : "root";
        lua_pop(L_, 1);

        lua_getfield(L_, -1, "password");
        mysql_password_ = lua_isstring(L_, -1) ? lua_tostring(L_, -1) : "";
        lua_pop(L_, 1);

        lua_getfield(L_, -1, "host");
        mysql_host_ = lua_isstring(L_, -1) ? lua_tostring(L_, -1) : "127.0.0.1";
        lua_pop(L_, 1);

        lua_getfield(L_, -1, "port");
        mysql_port_ = lua_isnumber(L_, -1) ? static_cast<int>(lua_tointeger(L_, -1)) : 3306;
        lua_pop(L_, 1);

        lua_getfield(L_, -1, "database");
        mysql_database_ = lua_isstring(L_, -1) ? lua_tostring(L_, -1) : "game_database";
        lua_pop(L_, 1);
    } else {
        spdlog::warn("MySQL configuration not found in config.lua");
    }
    lua_pop(L_, 1);

    return true;
}

std::string ConfigManager::getLogLevel() const {
    return log_level_;
}

double ConfigManager::getMemoryThreshold() const {
    return memory_threshold_;
}

int ConfigManager::getServerPort() const {
    return server_port_;
}

std::string ConfigManager::getMySQLUser() const {
    return mysql_user_;
}

std::string ConfigManager::getMySQLPassword() const {
    return mysql_password_;
}

std::string ConfigManager::getMySQLHost() const {
    return mysql_host_;
}

int ConfigManager::getMySQLPort() const {
    return mysql_port_;
}

std::string ConfigManager::getMySQLDatabase() const {
    return mysql_database_;
}
