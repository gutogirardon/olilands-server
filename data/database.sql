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
