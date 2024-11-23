#include "connectionpool.h"
#include <spdlog/spdlog.h>

ConnectionPool::ConnectionPool(const std::string& host, const std::string& user, const std::string& password,
                               const std::string& database, unsigned int port, std::size_t poolSize, std::size_t maxPoolSize)
    : currentSize_(0), maxSize_(maxPoolSize), host_(host), user_(user),
      password_(password), database_(database), port_(port) {
    for (std::size_t i = 0; i < poolSize; ++i) {
        MYSQL* conn = createConnection();
        if (conn) {
            pool_.push(conn);
            ++currentSize_;
        }
    }
    spdlog::info("MySQL connection created.");
    spdlog::info("Connection pool initialized with {} connections.", currentSize_);
}

ConnectionPool::~ConnectionPool() {
    while (!pool_.empty()) {
        MYSQL* conn = pool_.front();
        pool_.pop();
        mysql_close(conn);
    }
}

MYSQL* ConnectionPool::createConnection() {
    MYSQL* conn = mysql_init(nullptr);
    if (conn == nullptr) {
        spdlog::error("Failed to initialize MySQL.");
        return nullptr;
    }

    if (!mysql_real_connect(conn, host_.c_str(), user_.c_str(), password_.c_str(),
                            database_.c_str(), port_, nullptr, 0)) {
        spdlog::error("MySQL connection error: {}", mysql_error(conn));
        mysql_close(conn);
        return nullptr;
    }
    return conn;
}

MYSQL* ConnectionPool::acquire(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(poolMutex_);
    if (!poolCond_.wait_for(lock, timeout, [this]() { return !pool_.empty(); })) {
        // Attempt to create a new connection if possible
        if (currentSize_ < maxSize_) {
            MYSQL* conn = createConnection();
            if (conn) {
                ++currentSize_;
                return conn;
            }
        }
        spdlog::warn("Timeout while trying to acquire a connection from the pool.");
        return nullptr;
    }

    MYSQL* conn = pool_.front();
    pool_.pop();
    spdlog::info("Connection acquired. Available connections: {}", pool_.size());
    return conn;
}

void ConnectionPool::release(MYSQL* conn) {
    {
        std::lock_guard<std::mutex> lock(poolMutex_);
        pool_.push(conn);
    }
    poolCond_.notify_one();
    spdlog::info("Connection released. Available connections: {}", pool_.size());
}
