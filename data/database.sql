CREATE TABLE IF NOT EXISTS user_account (
                                            id INT AUTO_INCREMENT PRIMARY KEY,
                                            name VARCHAR(50) NOT NULL,
    password VARCHAR(255) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    premiumdays INT DEFAULT 0,
    premiumdays_purchased INT DEFAULT 0,
    type INT DEFAULT 0,
    coins INT DEFAULT 0,
    coins_transferable INT DEFAULT 0,
    data_criacao DATETIME DEFAULT CURRENT_TIMESTAMP,
    data_ultima_atualizacao DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    data_delecao DATETIME NULL
    );
