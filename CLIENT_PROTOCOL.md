# Documentação do Protocolo Cliente-Servidor

Este documento descreve o protocolo de comunicação entre o cliente e o servidor em nossa aplicação MMORPG. Detalha os comandos enviados pelo cliente e as respostas correspondentes do servidor. Este protocolo foi projetado para facilitar futuras implementações e garantir uma comunicação clara entre os componentes cliente e servidor.

---

## Índice

- [Estados da Sessão](#estados-da-sessão)
- [Lista de Comandos](#lista-de-comandos)
- [Formatos de Mensagens](#formatos-de-mensagens)
    - [Comandos de Autenticação](#comandos-de-autenticação)
    - [Comandos de Seleção de Personagem](#comandos-de-seleção-de-personagem)
    - [Comandos In-Game](#comandos-in-game)
- [Respostas do Servidor](#respostas-do-servidor)
- [Tratamento de Erros](#tratamento-de-erros)
- [Exemplo de Fluxo](#exemplo-de-fluxo)
- [Notas Adicionais](#notas-adicionais)

---

## Estados da Sessão

A comunicação cliente-servidor é dividida em diferentes estados de sessão, cada um aceitando comandos específicos:

1. **Não Autenticado**: O cliente deve se autenticar.
2. **Autenticado**: O cliente pode realizar seleção e criação de personagem.
3. **InGame**: O cliente está no mundo do jogo e pode executar ações in-game.

---

## Lista de Comandos

### Comandos do Protocolo

Os comandos do protocolo são identificados por valores inteiros específicos. Estes comandos são enviados pelo cliente ao servidor.

| Comando                       | Valor |
|-------------------------------|-------|
| LOGIN_REQUEST                 | 1     |
| CREATE_CHARACTER              | 4     |
| REQUEST_CHARACTER_LIST        | 5     |
| SELECT_CHARACTER              | 6     |
| MOVE_CHARACTER                | 7     |
| PING                          | 3     |
| LOGOUT                        | 2     |
| COMANDO_DESCONHECIDO          | Outros|

---

## Formatos de Mensagens

### Comandos de Autenticação

#### 1. **Solicitação de Login**

- **Comando**: `LOGIN_REQUEST` (Valor: `1`)
- **Formato da Mensagem do Cliente**:
- [1][Username][0][Password][0]
- `[1]`: Identificador de comando para solicitação de login.
- `[Username]`: String do nome de usuário, terminada em null.
- `[0]`: Terminador null.
- `[Password]`: String da senha, terminada em null.
- `[0]`: Terminador null.

- **Respostas do Servidor**:
- **Sucesso**: `"Login successful"`
- **Falha**: *Conexão é fechada pelo servidor.*

---

### Comandos de Seleção de Personagem

#### 2. **Solicitar Lista de Personagens**

- **Comando**: `REQUEST_CHARACTER_LIST` (Valor: `5`)
- **Formato da Mensagem do Cliente**:
- [5]
- **Resposta do Servidor**:
- "CharacterList|Nome1,Vocação1,Nível1;Nome2,Vocação2,Nível2;..."

- O servidor envia uma string começando com `"CharacterList|"`, seguida por uma lista de personagens separados por ponto e vírgula. Cada personagem é representado por seu nome, vocação e nível, separados por vírgulas.

#### 3. **Criar Personagem**

- **Comando**: `CREATE_CHARACTER` (Valor: `4`)
- **Formato da Mensagem do Cliente**:
- [4][NomeDoPersonagem][0][Vocação][0]

- `[4]`: Identificador de comando para criação de personagem.
- `[NomeDoPersonagem]`: String do nome desejado para o personagem, terminada em null.
- `[0]`: Terminador null.
- `[Vocação]`: String da vocação/classe, terminada em null.
- `[0]`: Terminador null.

- **Respostas do Servidor**:
- **Sucesso**: `"Character created successfully"`
- **Falha**: `"Character creation failed"`

#### 4. **Selecionar Personagem**

- **Comando**: `SELECT_CHARACTER` (Valor: `6`)
- **Formato da Mensagem do Cliente**:
- [6][NomeDoPersonagem][0]

- `[6]`: Identificador de comando para seleção de personagem.
- `[NomeDoPersonagem]`: String do nome do personagem a ser selecionado, terminada em null.
- `[0]`: Terminador null.

- **Respostas do Servidor**:
- **Sucesso**: `"CharacterSelected|Warrior|100,200,0"` (posicao)
- **Falha**: `"CharacterSelectionFailed"`

---

### Comandos In-Game

#### 5. **Mover Personagem**

- **Comando**: `MOVE_CHARACTER` (Valor: `7`)
- **Formato da Mensagem do Cliente**:
- [7][DeltaX][DeltaY]

- `[7]`: Identificador de comando para mover o personagem.
- `[DeltaX]`: Byte com sinal representando o movimento no eixo X (`-1`, `0`, `1`).
- `[DeltaY]`: Byte com sinal representando o movimento no eixo Y (`-1`, `0`, `1`).

- **Respostas do Servidor**:
- **Sucesso**: *Atualização de posição enviada para jogadores próximos (tratado internamente).*
- **Falha**: `"MovementBlocked"`

#### 6. **Ping**

- **Comando**: `PING` (Valor: `3`)
- **Formato da Mensagem do Cliente**:
- [3]
- **Resposta do Servidor**:
- "Pong"

#### 7. **Logout**

- **Comando**: `LOGOUT` (Valor: `2`)
- **Formato da Mensagem do Cliente**:
- [2]
- **Resposta do Servidor**:
- *O servidor salva a posição do jogador e fecha a conexão.*

---

## Respostas do Servidor

### Respostas Gerais

- **Login Bem-sucedido**:
- "Login successful"


- **Lista de Personagens**:
- "CharacterList|Nome1,Vocação1,Nível1;Nome2,Vocação2,Nível2;..."


- **Criação de Personagem Bem-sucedida**:
- "Character created successfully"


- **Falha na Criação de Personagem**:
- "Character creation failed"


- **Seleção de Personagem Bem-sucedida**:
- "CharacterSelected|NomeDoPersonagem"


- **Falha na Seleção de Personagem**:
- "CharacterSelectionFailed"


- **Resposta ao Ping**:
- "Pong"



- **Movimento Bloqueado**:
- "MovementBlocked"


### Notas

- **Atualizações de Posição**:

- Quando um jogador se move com sucesso, o servidor lida internamente com o envio de atualizações de posição para jogadores próximos. Isso não é comunicado diretamente ao cliente que se moveu, a menos que implementado.

- **Desconexão**:

- Se o servidor fechar a conexão devido a falha de autenticação ou logout, o cliente receberá um evento de conexão encerrada.

---

## Tratamento de Erros

- **Comandos Inválidos**:

- Se o servidor receber um comando desconhecido ou inválido, ele registra um erro e continua recebendo dados do cliente.

- **Falha de Autenticação**:

- O servidor fecha a conexão se a autenticação falhar.

- **Movimento para Posições Inválidas**:

- Se um jogador tentar se mover para uma posição não caminhável, o servidor responde com `"MovementBlocked"`.

- **Erros de Estado da Sessão**:

- Se uma ação for tentada fora do estado de sessão apropriado, o servidor pode fechar a conexão ou ignorar o comando.

---

## Exemplo de Fluxo

1. **Cliente se conecta ao servidor.**

2. **Autenticação**:

- **Cliente** envia:

  ```
  [1][Username][0][Password][0]
  ```

- **Servidor** responde:

  ```
  "Login successful"
  ```

3. **Solicitar Lista de Personagens**:

- **Cliente** envia:

  ```
  [5]
  ```

- **Servidor** responde:

  ```
  "CharacterList|Herói,Guerreiro,10;Mago,Feiticeiro,8;"
  ```

4. **Criar Personagem**:

- **Cliente** envia:

  ```
  [4][NovoPersonagem][0][Vocação][0]
  ```

- **Servidor** responde:

  ```
  "Character created successfully"
  ```

5. **Selecionar Personagem**:

- **Cliente** envia:

  ```
  [6][Herói][0]
  ```

- **Servidor** responde:

  ```
  "CharacterSelected|Herói"
  ```

6. **Movimento In-Game**:

- **Cliente** envia comando de mover para cima:

  ```
  [7][0][255]  // DeltaX = 0, DeltaY = -1
  ```

- **Servidor**:

    - Se o movimento for válido, atualiza a posição internamente.
    - Se o movimento for bloqueado, responde:

      ```
      "MovementBlocked"
      ```

7. **Ping**:

- **Cliente** envia:

  ```
  [3]
  ```

- **Servidor** responde:

  ```
  "Pong"
  ```

8. **Logout**:

- **Cliente** envia:

  ```
  [2]
  ```

- **Servidor** fecha a conexão.

---

## Notas Adicionais

- **Codificação das Mensagens**:

- Todas as strings enviadas nas mensagens são terminadas em null.
- Comandos são identificados por um único byte no início da mensagem.
- Deltas de movimento são bytes com sinal (`int8_t`).


- **Comunicação Assíncrona**:
- O servidor lida com mensagens de forma assíncrona, e os clientes devem estar preparados para lidar com respostas do servidor que podem chegar fora da ordem em relação às suas solicitações.


- **Timeouts de Sessão**:
- Se o cliente não se autenticar dentro de um determinado período após conectar, o servidor pode fechar a conexão.

---

Esta documentação do protocolo deve servir como referência para desenvolvedores implementando funcionalidades no lado do cliente e para manter a consistência em futuros desenvolvimentos no lado do servidor.

