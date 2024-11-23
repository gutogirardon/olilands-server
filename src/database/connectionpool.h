#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include <mysql/mysql.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <memory>
#include <chrono>

class ConnectionPool {
public:
    ConnectionPool(const std::string& host, const std::string& user, const std::string& password,
                  const std::string& database, unsigned int port, std::size_t poolSize, std::size_t maxPoolSize);
    ~ConnectionPool();

    MYSQL* acquire(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    void release(MYSQL* conn);

private:
    std::queue<MYSQL*> pool_;
    std::mutex poolMutex_;
    std::condition_variable poolCond_;
    std::size_t currentSize_;
    std::size_t maxSize_;
    std::string host_;
    std::string user_;
    std::string password_;
    std::string database_;
    unsigned int port_;

    MYSQL* createConnection();
};

#endif // CONNECTIONPOOL_H
