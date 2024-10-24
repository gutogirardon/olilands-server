-- data/config.lua

config = {
    log_level = "info",          -- Níveis possíveis: "debug", "info", "warn", "error"
    memory_threshold = 80.0,     -- Threshold de memória em porcentagem
    server_port = 7272,          -- Porta do servidor

    mysql = {
        user = "root",           -- Nome do usuário do MySQL
        password = "102030",         -- Senha do usuário do MySQL
        host = "127.0.0.1",             -- IP do servidor MySQL
        port = 3306,                    -- Porta do servidor MySQL
        database = "olilandsdatabase"      -- Nome da database
    }
}
