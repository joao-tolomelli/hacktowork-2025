#include <Arduino.h>
#include "secrets.h"
#include "FlowSensor.h"
#include "NetworkManager.h"

// --- Definição da Estrutura da Mensagem MQTT ---
struct MensagemMQTT {
    char topico[40];
    char payload[20];
};

// --- Objetos Globais ---
NetworkManager network(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER_IP, MQTT_PORT);
FlowSensor sensorChuveiro(25, 450.0);

// --- Handles do FreeRTOS ---
QueueHandle_t filaMQTT;
TaskHandle_t handleTaskSensor;
TaskHandle_t handleTaskRede;

// ============================================================
// TASK 1: GERENTE DE REDE (Consumidor)
// Responsável apenas por manter Wi-Fi/MQTT e despachar mensagens
// ============================================================
void taskRede(void * parameter) {
    network.begin(); // Conecta inicial

    MensagemMQTT msgRecebida;

    for(;;) {
        // Mantém a conexão viva
        network.update();

        // Verifica se há mensagens na fila para enviar
        if (xQueueReceive(filaMQTT, &msgRecebida, pdMS_TO_TICKS(10)) == pdTRUE) {
            
            // Se chegou aqui, tem mensagem para despachar
            if (network.isConnected()) {
                network.publish(msgRecebida.topico, msgRecebida.payload);
                Serial.printf("[Rede] Enviado: %s -> %s\n", msgRecebida.topico, msgRecebida.payload);
            } else {
                Serial.println("[Rede] Erro: Sem conexão para enviar mensagem.");
            }
        }
    }
}

// ============================================================
// TASK 2: MONITORAMENTO DE SENSOR (Produtor)
// Responsável apenas pela leitura do hardware
// ============================================================
void taskSensor(void * parameter) {
    const unsigned long TIMEOUT_BANHO = 15000;
    const unsigned long INTERVALO_ENVIO = 1000;

    unsigned long pulsosAnteriores = 0;
    unsigned long ultimaAtividade = 0;
    unsigned long ultimoEnvio = 0;
    bool banhoAtivo = false;
    
    // Buffer temporário para criar a mensagem
    MensagemMQTT msgEnviar; 

    for(;;) {
        // Coleta dados
        unsigned long pulsosAtuais = sensorChuveiro.getPulsosRaw();
        float volumeAtual = sensorChuveiro.getVolume();
        unsigned long agora = millis();

        // Detecta Fluxo
        if (pulsosAtuais > pulsosAnteriores) {
            ultimaAtividade = agora;
            pulsosAnteriores = pulsosAtuais;

            if (!banhoAtivo) {
                banhoAtivo = true;
                sensorChuveiro.reset();
                volumeAtual = 0;
                pulsosAnteriores = 0;
                
                Serial.println(">>> SESSÃO INICIADA <<<");
                
                // Prepara mensagem e joga na fila
                strcpy(msgEnviar.topico, "chuveiro/status");
                strcpy(msgEnviar.payload, "INICIO");
                xQueueSend(filaMQTT, &msgEnviar, portMAX_DELAY);
            }
        }

        // Lógica do Banho
        if (banhoAtivo) {
            // Timer de Telemetria
            if (agora - ultimoEnvio >= INTERVALO_ENVIO) {
                ultimoEnvio = agora;

                // Formata dados
                strcpy(msgEnviar.topico, "chuveiro/telemetria");
                snprintf(msgEnviar.payload, 20, "%.3f", volumeAtual);
                
                // Envia para a fila (sem bloquear muito se a fila estiver cheia)
                xQueueSend(filaMQTT, &msgEnviar, pdMS_TO_TICKS(10));
            }

            // Timeout / Fim
            if (agora - ultimaAtividade > TIMEOUT_BANHO) {
                Serial.println(">>> SESSÃO FINALIZADA <<<");
                
                /*
                // Envia Total
                strcpy(msgEnviar.topico, "chuveiro/total_final");
                snprintf(msgEnviar.payload, 20, "%.3f", volumeAtual);
                xQueueSend(filaMQTT, &msgEnviar, portMAX_DELAY);
                */

                // Envia Status Fim
                strcpy(msgEnviar.topico, "chuveiro/status");
                strcpy(msgEnviar.payload, "FIM");
                xQueueSend(filaMQTT, &msgEnviar, portMAX_DELAY);

                banhoAtivo = false;
                sensorChuveiro.reset();
                pulsosAnteriores = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

void setup() {
    Serial.begin(115200);
    sensorChuveiro.begin();

    // Cria a Fila
    // Capacidade para 10 mensagens. Se a rede cair, acumulamos até 10 leituras antes de perder dados.
    filaMQTT = xQueueCreate(10, sizeof(MensagemMQTT));

    if (filaMQTT == NULL) {
        Serial.println("Erro ao criar fila!");
        while(1);
    }

    // Cria as Tasks
    xTaskCreatePinnedToCore(
        taskRede, "NetTask", 4096, NULL, 1, &handleTaskRede, 0
    );

    xTaskCreatePinnedToCore(
        taskSensor, "SensorTask", 4096, NULL, 1, &handleTaskSensor, 1
    );
}

void loop() {
    vTaskDelete(NULL);
}