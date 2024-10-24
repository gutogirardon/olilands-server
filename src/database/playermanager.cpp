#include "playermanager.h"
#include <spdlog/spdlog.h>

PlayerManager::PlayerManager(DatabaseManager& dbManager)
    : dbManager_(dbManager) {}

bool PlayerManager::createCharacter(const std::string& name, const std::string& vocation, int account_id) {
    MYSQL* conn = dbManager_.getConnection();

    if (!dbManager_.isConnected()) {
        spdlog::error("Failed to connect to the database.");
        return false;
    }

    // Consultar quantos personagens a conta já tem
    std::string countQuery = "SELECT COUNT(*) FROM players WHERE account_id = " + std::to_string(account_id);
    if (mysql_query(conn, countQuery.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int characterCount = row ? std::stoi(row[0]) : 0;
    mysql_free_result(result);

    // Limitar o número de personagens a 5
    if (characterCount >= 5) {
        spdlog::error("Account {} has reached the character limit.", account_id);
        return false;
    }

    // Inserir novo personagem
    std::string insertQuery = "INSERT INTO players (name, vocation, account_id, health, max_health, mana, max_mana, level, experience) "
                              "VALUES ('" + name + "', '" + vocation + "', " + std::to_string(account_id) + ", 100, 100, 100, 100, 1, 0)";

    if (mysql_query(conn, insertQuery.c_str())) {
        spdlog::error("MySQL insert error: {}", mysql_error(conn));
        return false;
    }

    spdlog::info("Character {} created successfully for account {}", name, account_id);
    return true;
}

std::vector<CharacterInfo> PlayerManager::getCharactersForAccount(int account_id) {
    std::vector<CharacterInfo> characters;
    MYSQL* conn = dbManager_.getConnection();

    if (!dbManager_.isConnected()) {
        spdlog::error("Failed to connect to the database.");
        return characters;
    }

    std::string query = "SELECT id, name, vocation, level FROM players WHERE account_id = " + std::to_string(account_id);

    if (mysql_query(conn, query.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        return characters;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        spdlog::error("MySQL store result error: {}", mysql_error(conn));
        return characters;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        CharacterInfo charInfo;
        charInfo.id = std::stoi(row[0]);
        charInfo.name = row[1];
        charInfo.vocation = row[2];
        charInfo.level = std::stoi(row[3]);
        characters.push_back(charInfo);
    }

    mysql_free_result(result);
    return characters;
}

CharacterInfo PlayerManager::getCharacterInfo(const std::string& characterName, int account_id) {
    CharacterInfo charInfo;
    MYSQL* conn = dbManager_.getConnection();

    if (!dbManager_.isConnected()) {
        spdlog::error("Failed to connect to the database.");
        return charInfo;
    }

    std::string query = "SELECT id, name, vocation, level FROM players WHERE name = '" + characterName + "' AND account_id = " + std::to_string(account_id) + " LIMIT 1";

    if (mysql_query(conn, query.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        return charInfo;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        spdlog::error("MySQL store result error: {}", mysql_error(conn));
        return charInfo;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        charInfo.id = std::stoi(row[0]);
        charInfo.name = row[1];
        charInfo.vocation = row[2];
        charInfo.level = std::stoi(row[3]);
    } else {
        spdlog::error("Character {} not found for account {}", characterName, account_id);
    }

    mysql_free_result(result);
    return charInfo;
}