#include "useraccountmanager.h"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

UserAccountManager::UserAccountManager(DatabaseManager& dbManager)
    : dbManager_(dbManager) {}

bool UserAccountManager::validateLogin(const std::string& username, const std::string& password, int& account_id) {
    MYSQL* conn = dbManager_.getConnection();  // Adquire uma conexão do pool

    if (!conn) {
        spdlog::error("No available database connection.");
        return false;
    }

    std::string query = "SELECT id, password_hash FROM user_account WHERE username = '" + username + "' LIMIT 1";

    if (mysql_query(conn, query.c_str())) {
        spdlog::error("MySQL query error: {}", mysql_error(conn));
        dbManager_.releaseConnection(conn);  // Libera a conexão em caso de erro
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        spdlog::error("MySQL store result error: {}", mysql_error(conn));
        dbManager_.releaseConnection(conn);  // Libera a conexão em caso de erro
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == nullptr) {
        spdlog::info("Invalid username or password.");
        mysql_free_result(result);
        dbManager_.releaseConnection(conn);  // Libera a conexão
        return false;
    }

    account_id = std::stoi(row[0]);
    std::string stored_password_hash = row[1];
    std::string received_password_hash = hashPassword(password);

    mysql_free_result(result);
    dbManager_.releaseConnection(conn);  // Libera a conexão após uso

    if (stored_password_hash == received_password_hash) {
        spdlog::info("User {} authenticated successfully.", username);
        return true;
    }

    spdlog::warn("Invalid password for user: {}", username);
    return false;
}

std::string UserAccountManager::hashPassword(const std::string& password) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if(context == nullptr) {
        throw std::runtime_error("Failed to create EVP_MD_CTX.");
    }

    if(!EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to initialize EVP for SHA256.");
    }

    if(!EVP_DigestUpdate(context, password.c_str(), password.size())) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to update EVP digest.");
    }

    if(!EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Failed to finalize EVP digest.");
    }

    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for(unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}
