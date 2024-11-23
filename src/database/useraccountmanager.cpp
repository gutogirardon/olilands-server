#include "useraccountmanager.h"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>
#include "connectionguard.h"

UserAccountManager::UserAccountManager(DatabaseManager& dbManager)
    : dbManager_(dbManager) {}

UserAccountManager::~UserAccountManager() {}

bool UserAccountManager::validateLogin(const std::string& username, const std::string& password, int& account_id) {
    try {
        // Adquirir conexão usando ConnectionGuard
        ConnectionGuard connGuard(dbManager_);
        MYSQL* conn = connGuard.get();

        // Verificar se a conexão foi adquirida
        if (!conn) {
            spdlog::error("Falha ao adquirir uma conexão com o banco de dados.");
            return false;
        }

        // Construir a consulta SQL
        std::string query = "SELECT id, password_hash FROM user_account WHERE username = '" + username + "' LIMIT 1";

        // Executar a consulta
        if (mysql_query(conn, query.c_str())) {
            spdlog::error("Erro na consulta MySQL: {}", mysql_error(conn));
            return false;
        }

        // Obter os resultados da consulta
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == nullptr) {
            spdlog::error("Erro ao armazenar o resultado da consulta MySQL: {}", mysql_error(conn));
            return false;
        }

        // Buscar a primeira linha do resultado
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row == nullptr) {
            spdlog::info("Nome de usuário ou senha inválidos para '{}'.", username);
            mysql_free_result(result);
            return false;
        }

        // Extrair os dados do resultado
        account_id = std::stoi(row[0]);
        std::string stored_password_hash = row[1];
        std::string received_password_hash = hashPassword(password);

        // Liberar o resultado da consulta
        mysql_free_result(result);

        // Comparar o hash da senha armazenada com o hash da senha recebida
        if (stored_password_hash == received_password_hash) {
            spdlog::info("Usuário '{}' autenticado com sucesso.", username);
            return true;
        }

        spdlog::warn("Senha inválida para o usuário '{}'.", username);
        return false;
    } catch (const std::exception& e) {
        spdlog::error("Exceção em validateLogin: {}", e.what());
        return false;
    }
}

std::string UserAccountManager::hashPassword(const std::string& password) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        throw std::runtime_error("Falha ao criar EVP_MD_CTX.");
    }

    if (!EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Falha ao inicializar EVP para SHA256.");
    }

    if (!EVP_DigestUpdate(context, password.c_str(), password.size())) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Falha ao atualizar o digest EVP.");
    }

    if (!EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Falha ao finalizar o digest EVP.");
    }

    EVP_MD_CTX_free(context);

    // Converter o hash para uma string hexadecimal
    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}
