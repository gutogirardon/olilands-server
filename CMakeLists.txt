cmake_minimum_required(VERSION 3.14)
project(olilands_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar Boost
find_package(Boost REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# Usar FetchContent para baixar spdlog e tmxlite
include(FetchContent)

# spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# tmxlite
FetchContent_Declare(
        tmxlite
        GIT_REPOSITORY https://github.com/fallahn/tmxlite.git
        GIT_TAG v1.4.4
        SOURCE_SUBDIR tmxlite
)
FetchContent_MakeAvailable(tmxlite)

# Habilitar pkg-config
find_package(PkgConfig REQUIRED)

# Encontrar LuaJIT usando pkg-config
pkg_check_modules(LUAJIT REQUIRED luajit)

if(NOT LUAJIT_FOUND)
    message(FATAL_ERROR "LuaJIT not found!")
endif()

# Encontrar a biblioteca LuaJIT usando find_library
find_library(LUAJIT_LIB luajit-5.1 PATHS ${LUAJIT_LIBRARY_DIRS} /opt/homebrew/lib)

if(NOT LUAJIT_LIB)
    message(FATAL_ERROR "LuaJIT library 'luajit-5.1' not found!")
endif()

# Detectar sistema operacional
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    # Configuração para macOS usando Homebrew para MySQL C API
    find_path(MYSQL_INCLUDE_DIR mysql/mysql.h PATHS /opt/homebrew/opt/mysql-client/include)
    find_library(MYSQL_LIB NAMES mysqlclient PATHS /opt/homebrew/opt/mysql-client/lib)

    # Localizar OpenSSL via Homebrew
    find_package(OpenSSL REQUIRED)
    if(NOT OpenSSL_FOUND)
        message(FATAL_ERROR "OpenSSL not found! Please install it via Homebrew.")
    endif()

    include_directories(${OPENSSL_INCLUDE_DIR})
    link_directories(${OPENSSL_LIBRARIES})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # Configuração para Linux (Ubuntu)
    find_path(MYSQL_INCLUDE_DIR mysql/mysql.h PATHS /usr/include /usr/local/include)
    find_library(MYSQL_LIB NAMES mysqlclient PATHS /usr/lib /usr/local/lib)

    # No Linux, encontrar o OpenSSL
    find_package(OpenSSL REQUIRED)
endif()

if(NOT MYSQL_INCLUDE_DIR OR NOT MYSQL_LIB)
    message(FATAL_ERROR "MySQL C API (libmysqlclient) not found! Please install it via Homebrew or apt.")
endif()

# Buscar todos os arquivos .cpp recursivamente na pasta src/
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp")

# Adicionar executável antes de definir include directories
add_executable(olilands_server ${SOURCES})

# Definir diretórios de inclusão
target_include_directories(olilands_server PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
        ${LUAJIT_INCLUDE_DIRS}
        ${MYSQL_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
)

# Linkar bibliotecas Boost, spdlog, LuaJIT, MySQL C API e tmxlite
target_link_libraries(olilands_server PRIVATE
        Boost::system
        spdlog::spdlog
        ${LUAJIT_LIB}
        ${MYSQL_LIB}
        OpenSSL::SSL
        OpenSSL::Crypto
        tmxlite
)

# Adicionar diretórios de linkagem do MySQL C API
link_directories(${MYSQL_LIB})
