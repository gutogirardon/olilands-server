#include "useraccountmanager.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

UserAccountManager::UserAccountManager(DatabaseManager& dbManager)
    : dbManager_(dbManager) {}

bool UserAccountManager::validateLogin(const std::string& username, const std::string& password) {
    MYSQL* conn = dbManager_.getConnection();

    if (!dbManager_.isConnected()) {
        spdlog::error("Failed to connect to the database.");
        return false;
    }

    std::string query = "SELECT password_hash FROM user_account WHERE username = '" + username + "' LIMIT 1";

    if (mysql_query(conn, query.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        spdlog::error("MySQL store result error: {}", mysql_error(conn));
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == nullptr) {
        spdlog::info("Invalid username or password.");
        mysql_free_result(result);
        return false;
    }

    std::string stored_password_hash = row[0];
    std::string received_password_hash = hashPassword(password);

    mysql_free_result(result);

    if (stored_password_hash == received_password_hash) {
        spdlog::info("User {} authenticated successfully.", username);
        return true;
    } else {
        spdlog::info("Invalid password for user: {}", username);
        return false;
    }
}

std::string UserAccountManager::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}
