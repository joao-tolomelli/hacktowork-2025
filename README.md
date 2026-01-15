# Hack to Work 2025

- Trilha: Sistemas embarcados, sustentabilidade.

## Proposta

Um sistema de monitoramento residencial de consumo de água, com alarmes.



## Arquitetura

O sistema é composto por duas partes principais: **sistema embarcado** e **aplicação web**, que se comunicam através do protocolo MQTT.

### Sistema embarcado

Componentes:

- ESP-32
- Sensor de Fluxo de Água YF-S201 1/2"
- LEDs
- Buzzer

#### Funcionamento

O firmware implementa um sistema de monitoramento de consumo de água para um chuveiro, utilizando um ESP-32, um sensor de fluxo de água, um LED e um buzzer. Ele opera da seguinte forma:

1. **Detecção de Fluxo de Água**:
   - O sistema entra em modo de espera até detectar fluxo de água através do sensor de fluxo YF-S201.
   - Quando o fluxo é detectado, o sistema "acorda" e inicia as tarefas de monitoramento.

2. **Monitoramento do Consumo**:
   - O sensor mede o volume de água consumido em intervalos de 5 segundos, acumulando os pulsos do sensor e convertendo-os em litros.
   - Os dados de consumo são enviados periodicamente para um servidor MQTT no tópico `chuveiro/telemetria`.

3. **Alertas de Limite**:
   - O sistema recebe mensagens MQTT no tópico `chuveiro/limite` para indicar o estado do consumo:
     - **"PROXIMO"**: Indica que o consumo está próximo do limite. O LED é ligado como alerta visual.
     - **"ESTOURO"**: Indica que o limite foi ultrapassado. O LED é desligado e o buzzer é ativado como alerta sonoro.
     - **"NORMAL" ou "RESET"**: Indica que o consumo foi normalizado ou um novo banho começou. O LED e o buzzer são desligados.

4. **Envio de Status**:
   - Quando o fluxo de água começa, o sistema publica a mensagem `INICIO` no tópico `chuveiro/status`.
   - Quando o fluxo de água termina (após 10 segundos sem pulsos), o sistema publica a mensagem `FIM` no mesmo tópico.

5. **Gerenciamento de Tarefas**:
   - O firmware utiliza o FreeRTOS para gerenciar tarefas de rede e leitura do sensor de forma assíncrona.
   - As tarefas são pausadas quando não há fluxo de água e retomadas automaticamente quando o fluxo é detectado.

6. **Reconexão Automática**:
   - O sistema gerencia a conexão Wi-Fi e MQTT, reconectando automaticamente em caso de desconexão e reassinando os tópicos necessários.

Em resumo, o firmware monitora o consumo de água, envia dados para um servidor MQTT, e utiliza alertas visuais e sonoros para indicar o estado do consumo em relação ao limite configurado.

### Aplicação Web

#### Componentes

- Banco Redis, para operações Pub/Sub;
- Aplicação Subscriber (Python), inscrita nos tópicos do servidor MQTT;
- Backend (Python, Flask): manipula os dados em banco de dados SQLite, gerencia a conexão de Websocket com o Frontend.
- Frontend (JavaScript, React): interface web do sistema, mostra uma dashboard com os banhos registrados, com atualizações em tempo real.

#### Funcionamento

1. A aplicação Subscriber escuta os tópicos relevantes no broker MQTT e envia uma notificação _pub/sub_ para o banco Redis.

2. Uma thread secundária do Backend escuta os canais de Pub/Sub do Redis e altera o estado do sistema de acordo, notificando todos os clientes conectados ao servidor sobre as mudanças.

3. Os clientes (frontend) requisitam os dados mais recentes e atualizam a página.

4. Se o consumo daquele banho específico exceder o limite estipulado, o backend envia uma notificação pub/sub ao Redis especificando o nível de alerta.

5. A aplicação Subscriber recebe a notificação de alerta via Redis e transmite ao servidor MQTT, para que o sistema embarcado receba a mensagem.

## Como rodar

Clone o repositório:

```sh
git clone https://github.com/joao-tolomelli/hacktowork-2025.git
```

Suba os containers:

```sh
docker compose up -d
```

Rode o frontend:

```sh
cd client/hacktowork2025-client
npm i
npm run dev
```

Acesse a aplicação em `http://localhost:5173`
