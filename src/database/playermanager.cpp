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
