#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include <mysql/mysql.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

class ConnectionPool {
public:
    // Inicializa o pool com um número fixo de conexões
    ConnectionPool(const std::string& host, const std::string& user, const std::string& password,
                   const std::string& database, unsigned int port, std::size_t poolSize);
    ~ConnectionPool();

    // Adquire uma conexão do pool
    MYSQL* acquire();
    // Devolve a conexão ao pool
    void release(MYSQL* conn);

private:
    std::queue<MYSQL*> pool_;  // Fila de conexões disponíveis
    std::mutex poolMutex_;     // Mutex para proteger o acesso ao pool
    std::condition_variable poolCond_;  // Para sinalizar quando uma conexão for liberada

    // Função auxiliar para criar uma nova conexão
    MYSQL* createConnection(const std::string& host, const std::string& user, const std::string& password,
                            const std::string& database, unsigned int port);
};

#endif // CONNECTIONPOOL_H
