### 1. Install the required software
```
sudo apt update && sudo apt dist-upgrade -y
```


```
sudo apt install git cmake build-essential autoconf libtool ca-certificates curl zip unzip tar pkg-config ninja-build ccache linux-headers-$(uname -r) -y
```

### 2. Update cmake

```
sudo apt remove --purge cmake -y
hash -r
sudo apt install snapd -y
sudo snap install cmake --classic
cmake --version
```

### 3. Update gcc

ToDo

### 4. Install boost + spdlog
```
sudo apt install libboost-all-dev
sudo apt install libspdlog-dev
sudo apt install libluajit-5.1-dev
sudo apt install build-essential gcc make
sudo apt install libssl-dev
sudo apt install mariadb-server mariadb-client libmariadb-dev
sudo apt install libmariadb-dev-compat libmariadb-dev
```

### 5. Parte web
```
sudo apt install mariadb-server mariadb-client -y

sudo systemctl start mariadb
sudo systemctl enable mariadb

sudo mysql_secure_installation

sudo apt install nginx -y
sudo systemctl start nginx
sudo systemctl enable nginx

sudo apt install php-fpm php-mysql php-zip php-json php-mbstring php-curl php-gd php-xml php-cli -y
sudo systemctl restart php7.4-fpm

sudo apt install phpmyadmin -y
sudo ln -s /usr/share/phpmyadmin /var/www/html/phpmyadmin

sudo nano /etc/nginx/sites-available/phpmyadmin


server {
    listen 80;
    server_name your_domain_or_ip;

    root /var/www/html;
    index index.php index.html index.htm;

    location / {
        try_files $uri $uri/ =404;
    }

    location ~ \.php$ {
        include snippets/fastcgi-php.conf;
        fastcgi_pass unix:/var/run/php/php7.4-fpm.sock; # Verifique a versão do PHP instalada
        fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
        include fastcgi_params;
    }

    location ~ /\.ht {
        deny all;
    }

    location /phpmyadmin {
        alias /usr/share/phpmyadmin;
        index index.php;
    }
}

sudo ln -s /etc/nginx/sites-available/phpmyadmin /etc/nginx/sites-enabled/
sudo nginx -t

sudo systemctl restart nginx



sudo ufw allow 7272/tcp
sudo ufw allow 7272/udp
sudo ufw allow 7272





```