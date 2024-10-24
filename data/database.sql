CREATE TABLE IF NOT EXISTS user_account (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    premium_days INT DEFAULT 0,
    premium_days_purchased INT DEFAULT 0,
    account_type INT DEFAULT 0,
    balance_coins INT DEFAULT 0,
    transferable_coins INT DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    deleted_at DATETIME NULL
    );

INSERT INTO user_account (username, password_hash, email, premium_days, premium_days_purchased, account_type, balance_coins, transferable_coins)
VALUES ('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 'admin@admin.com', 0, 0, 1, 100, 50);

CREATE TABLE IF NOT EXISTS players (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    vocation VARCHAR(50) NOT NULL,
    account_id INT NOT NULL,  -- Chave estrangeira para a tabela user_account
    health INT DEFAULT 100,  -- Vida atual
    max_health INT DEFAULT 100,  -- Vida máxima
    mana INT DEFAULT 100,  -- Mana atual
    max_mana INT DEFAULT 100,  -- Mana máxima
    look_body TINYINT DEFAULT 0,  -- Aparência do corpo
    look_feet TINYINT DEFAULT 0,  -- Aparência dos pés
    look_head TINYINT DEFAULT 0,  -- Aparência da cabeça
    look_legs TINYINT DEFAULT 0,  -- Aparência das pernas
    look_type TINYINT DEFAULT 0,  -- Aparência do tipo de personagem
    magic_level TINYINT DEFAULT 0,  -- Nível mágico
    level INT DEFAULT 1,  -- Nível do jogador
    experience BIGINT DEFAULT 0,  -- Experiência acumulada do jogador
    pos_x INT DEFAULT 0,  -- Posição no eixo X
    pos_y INT DEFAULT 0,  -- Posição no eixo Y
    pos_z INT DEFAULT 0,  -- Posição no eixo Z
    gender ENUM('male', 'female') NOT NULL,  -- Sexo do jogador
    blessed BOOLEAN DEFAULT FALSE,  -- Jogador está abençoado
    balance DECIMAL(10, 2) DEFAULT 0.00,  -- Saldo de dinheiro do jogador
    hours_played INT DEFAULT 0,  -- Horas jogadas
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,  -- Data de criação do personagem
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,  -- Data de atualização do personagem
    deleted_at DATETIME NULL,  -- Data de exclusão (para soft delete)

-- Chave estrangeira para a tabela user_account
    FOREIGN KEY (account_id) REFERENCES user_account(id) ON DELETE CASCADE
    );

