#include "usermanager.h"
#include <spdlog/spdlog.h>
#include <mysql/mysql.h>

UserManager::UserManager(DatabaseManager& dbManager)
    : dbManager_(dbManager) {}

bool UserManager::saveUserData(int userId, const std::string& data) {
    if (!dbManager_.isConnected()) {
        dbManager_.reconnect();
    }

    MYSQL* conn = dbManager_.getConnection();
    std::string query = "UPDATE user_account SET data = ? WHERE id = ?";

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        spdlog::error("MySQL statement initialization failed: {}", mysql_error(conn));
        return false;
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        spdlog::error("MySQL statement preparation failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    // Binding parameters
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    // Bind data (as string)
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)data.c_str();
    bind[0].buffer_length = data.length();

    // Bind user ID (as integer)
    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &userId;
    bind[1].buffer_length = sizeof(userId);

    if (mysql_stmt_bind_param(stmt, bind)) {
        spdlog::error("MySQL parameter binding failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    // Execute statement
    if (mysql_stmt_execute(stmt)) {
        spdlog::error("MySQL statement execution failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    spdlog::info("User data saved for user ID {}", userId);
    return true;
}

std::string UserManager::getUserData(int userId) {
    if (!dbManager_.isConnected()) {
        dbManager_.reconnect();
    }

    MYSQL* conn = dbManager_.getConnection();
    std::string query = "SELECT data FROM user_account WHERE id = ?";

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        spdlog::error("MySQL statement initialization failed: {}", mysql_error(conn));
        return "";
    }

    if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
        spdlog::error("MySQL statement preparation failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return "";
    }

    // Binding input parameters (user ID)
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &userId;
    bind[0].buffer_length = sizeof(userId);

    if (mysql_stmt_bind_param(stmt, bind)) {
        spdlog::error("MySQL parameter binding failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return "";
    }

    // Execute statement
    if (mysql_stmt_execute(stmt)) {
        spdlog::error("MySQL statement execution failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return "";
    }

    // Binding result
    MYSQL_BIND result[1];
    memset(result, 0, sizeof(result));

    char data[256];  // Buffer to hold result
    result[0].buffer_type = MYSQL_TYPE_STRING;
    result[0].buffer = data;
    result[0].buffer_length = sizeof(data);

    if (mysql_stmt_bind_result(stmt, result)) {
        spdlog::error("MySQL result binding failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return "";
    }

    // Fetch result
    if (mysql_stmt_fetch(stmt)) {
        spdlog::error("MySQL fetch failed: {}", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return "";
    }

    mysql_stmt_close(stmt);
    spdlog::info("Fetched data for user ID {}", userId);
    return std::string(data);
}
