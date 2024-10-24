#include "connectionpool.h"
#include <spdlog/spdlog.h>

ConnectionPool::ConnectionPool(const std::string& host, const std::string& user,
                               const std::string& password, const std::string& database,
                               unsigned int port, std::size_t poolSize) {
    for (std::size_t i = 0; i < poolSize; ++i) {
        MYSQL* conn = createConnection(host, user, password, database, port);
        if (conn) {
            pool_.push(conn);
        }
    }
}

ConnectionPool::~ConnectionPool() {
    while (!pool_.empty()) {
        MYSQL* conn = pool_.front();
        pool_.pop();
        mysql_close(conn);  // Fechar todas as conexões ao liberar o pool
    }
}

MYSQL* ConnectionPool::createConnection(const std::string& host, const std::string& user,
                                        const std::string& password, const std::string& database,
                                        unsigned int port) {
    MYSQL* conn = mysql_init(nullptr);
    if (conn == nullptr) {
        spdlog::error("MySQL initialization failed.");
        return nullptr;
    }

    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(),
                            database.c_str(), port, nullptr, 0)) {
        spdlog::error("MySQL connection error: {}", mysql_error(conn));
        mysql_close(conn);
        return nullptr;
                            }

    spdlog::info("MySQL connection created.");
    return conn;
}

MYSQL* ConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(poolMutex_);
    poolCond_.wait(lock, [this]() { return !pool_.empty(); });  // Espera até que uma conexão esteja disponível

    MYSQL* conn = pool_.front();
    pool_.pop();
    return conn;
}

void ConnectionPool::release(MYSQL* conn) {
    std::lock_guard<std::mutex> lock(poolMutex_);
    pool_.push(conn);
    poolCond_.notify_one();  // Notifica uma thread que está esperando por uma conexão
}
