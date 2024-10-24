CREATE TABLE IF NOT EXISTS user_account (
                                            id INT AUTO_INCREMENT PRIMARY KEY,
                                            username VARCHAR(50) NOT NULL,
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
