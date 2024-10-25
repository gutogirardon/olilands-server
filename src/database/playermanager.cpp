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

    // Limitar o número de personagens a 5
    std::string countQuery = "SELECT COUNT(*) FROM players WHERE account_id = " + std::to_string(account_id);
    if (mysql_query(conn, countQuery.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int characterCount = row ? std::stoi(row[0]) : 0;
    mysql_free_result(result);

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

    std::string query = "SELECT id, name, vocation, account_id, health, max_health, mana, max_mana, look_body, look_feet, look_head, "
                        "look_legs, look_type, magic_level, level, experience, pos_x, pos_y, pos_z, gender, blessed, balance, "
                        "hours_played FROM players WHERE name = '" + characterName + "' AND account_id = " + std::to_string(account_id) + " LIMIT 1";

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
        charInfo.account_id = std::stoi(row[3]);
        charInfo.health = std::stoi(row[4]);
        charInfo.max_health = std::stoi(row[5]);
        charInfo.mana = std::stoi(row[6]);
        charInfo.max_mana = std::stoi(row[7]);
        charInfo.look_body = std::stoi(row[8]);
        charInfo.look_feet = std::stoi(row[9]);
        charInfo.look_head = std::stoi(row[10]);
        charInfo.look_legs = std::stoi(row[11]);
        charInfo.look_type = std::stoi(row[12]);
        charInfo.magic_level = std::stoi(row[13]);
        charInfo.level = std::stoi(row[14]);
        charInfo.experience = std::stoll(row[15]);
        charInfo.pos_x = std::stoi(row[16]);
        charInfo.pos_y = std::stoi(row[17]);
        charInfo.pos_z = std::stoi(row[18]);
        charInfo.gender = row[19];
        charInfo.blessed = std::stoi(row[20]) != 0;
        charInfo.balance = std::stod(row[21]);
        charInfo.hours_played = std::stoi(row[22]);
    } else {
        spdlog::error("Character {} not found for account {}", characterName, account_id);
    }

    mysql_free_result(result);
    return charInfo;
}

std::tuple<int, int, int> PlayerManager::getPlayerPosition(int player_id) {
    MYSQL* conn = dbManager_.getConnection();

    if (!dbManager_.isConnected()) {
        spdlog::error("Failed to connect to the database.");
        return {0, 0, 0}; // Retorna posição padrão em caso de erro
    }

    std::string query = "SELECT pos_x, pos_y, pos_z FROM players WHERE id = " + std::to_string(player_id);

    if (mysql_query(conn, query.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        return {0, 0, 0};
    }

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);

    int pos_x = 0, pos_y = 0, pos_z = 0;
    if (row) {
        pos_x = std::stoi(row[0]);
        pos_y = std::stoi(row[1]);
        pos_z = std::stoi(row[2]);
    } else {
        spdlog::error("Player ID {} position not found", player_id);
    }

    mysql_free_result(result);
    return {pos_x, pos_y, pos_z};
}

bool PlayerManager::updatePlayerPosition(int player_id, int pos_x, int pos_y, int pos_z) {
    MYSQL* conn = dbManager_.getConnection();

    if (!dbManager_.isConnected()) {
        spdlog::error("Failed to connect to the database.");
        return false;
    }

    std::string query = "UPDATE players SET pos_x = " + std::to_string(pos_x) +
                        ", pos_y = " + std::to_string(pos_y) +
                        ", pos_z = " + std::to_string(pos_z) +
                        " WHERE id = " + std::to_string(player_id);

    if (mysql_query(conn, query.c_str())) {
        spdlog::error("MySQL update error: {}", mysql_error(conn));
        return false;
    }

    spdlog::info("Updated position for player {} to ({}, {}, {})", player_id, pos_x, pos_y, pos_z);
    return true;
}
