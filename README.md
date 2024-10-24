# Olilands Server

## Descrição

O **Olilands Server** é um servidor de jogos desenvolvido em C++ que utiliza **Boost.Asio** para comunicação assíncrona e **spdlog** para logging eficiente. Projetado para lidar com múltiplas conexões de clientes de forma escalável e robusta, o servidor é ideal para jogos que exigem alta performance e baixa latência.

## Funcionalidades

- **Comunicação Assíncrona**: Gerencia múltiplas conexões de clientes de forma eficiente usando Boost.Asio.
- **Logging Avançado**: Utiliza spdlog para logs detalhados com suporte a diferentes níveis de log.
- **Monitoramento de Memória**: Classe `memorymonitor` que monitora o uso de memória e emite alertas quando ultrapassa 80%.
- **Estrutura Modular**: Código organizado em pastas separadas para facilitar a manutenção e escalabilidade.
- **Configuração Dinâmica com CMake**: Build system configurado para incluir automaticamente todos os arquivos fonte.

## Arquitetura

A arquitetura do servidor está dividida nas seguintes classes principais:

- **Server**: Responsável por aceitar novas conexões de clientes.
- **Session**: Gerencia a comunicação com um cliente individual.
- **MemoryMonitor**: Monitora o uso de memória do servidor e loga alertas quando necessário.


## Requisitos

- **C++20**
- **CMake** (mínimo versão 3.14)
- **Boost** (componentes: system)
- **spdlog**
- **Ninja** (opcional, mas recomendado para builds mais rápidos)
- **Xcode Command Line Tools** (para macOS)

## Instalação

1. **Clonar o Repositório**

    ```bash
    git clone https://github.com/seu-usuario/olilands_server.git
    cd olilands_server
    ```

2. **Instalar Dependências**

    - **Boost** e **spdlog** podem ser instalados via Homebrew:

        ```bash
        brew install boost
        brew install spdlog
        ```

3. **Configurar o Build com CMake**

    ```bash
    mkdir build
    cd build
    cmake ..
    ```

4. **Compilar o Projeto**

    ```bash
    make
    ```

## Uso

Após a compilação, execute o servidor:

```bash
./olilands_server
