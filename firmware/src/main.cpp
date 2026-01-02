#include <Arduino.h>
#include "FlowSensor.h"
#include "NetworkManager.h"
#include "Buzzer.h"
#include "Led.h"
#include "secrets.h"

// --- Estruturas ---
struct MensagemMQTT {
    char topico[40];
    char payload[20];
};

// --- Objetos Globais ---
NetworkManager network(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER_IP, MQTT_PORT);
FlowSensor sensorChuveiro(25, 450.0);
Buzzer buzzerAlerta(26, 0); 
Led ledAviso(27);

// --- Handles RTOS ---
QueueHandle_t filaMQTT;
SemaphoreHandle_t semaforoFluxo;
TaskHandle_t handleTaskRede;
TaskHandle_t handleTaskSensor;

// --- Variável Global de Estado do Alerta ---
// Usada para o buzzer não ficar reiniciando o tom a cada mensagem recebida
bool buzzerAtivo = false;

// ============================================================
// CALLBACK MQTT (O que acontece quando chega mensagem)
// ============================================================
void callbackMQTT(char* topic, uint8_t* payload, unsigned int length) {
    String msg = "";
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
    
    Serial.printf("[MQTT] %s: %s\n", topic, msg.c_str());

    if (String(topic) == "chuveiro/limite") {
        
        // CENÁRIO 1: Aviso ("Está chegando perto") [cite: 32]
        // Comportamento: Liga LED, Buzzer desligado
        if (msg == "PROXIMO") {
            if (!ledAviso.estaLigado()) {
                Serial.println(">>> ALERTA VISUAL: Limite Próximo <<<");
                ledAviso.ligar();
            }
            // Garante que o buzzer esteja quieto
            if (buzzerAtivo) {
                buzzerAlerta.parar();
                buzzerAtivo = false;
            }
        } 
        
        // CENÁRIO 2: Estouro ("Passou do limite") [cite: 34]
        // Comportamento: Liga Buzzer, Desliga LED (conforme solicitado)
        else if (msg == "ESTOURO") {
            // Apaga o aviso visual
            if (ledAviso.estaLigado()) {
                ledAviso.desligar();
            }

            // Liga o aviso sonoro
            if (!buzzerAtivo) {
                Serial.println(">>> ALERTA SONORO: ESTOURO! <<<");
                buzzerAlerta.tocar(3000); // 3kHz
                buzzerAtivo = true;
            }
        } 
        
        // CENÁRIO 3: Reset ("Banho novo ou normalizado") [cite: 37]
        // Comportamento: Tudo desligado
        else if (msg == "NORMAL" || msg == "RESET") {
            Serial.println(">>> Status Normalizado. <<<");
            buzzerAlerta.parar();
            ledAviso.desligar();
            buzzerAtivo = false;
        }
    }
}

// ============================================================
// TASK DE REDE 
// ============================================================
void taskRede(void * parameter) {
    MensagemMQTT msg;
    
    // Configura o Callback ANTES de conectar
    network.setCallback(callbackMQTT);
    network.begin();

    bool estavaConectado = false;

    for(;;) {
        network.update();

        // Lógica de Reconexão e Assinatura (Subscribe)
        if (network.isConnected()) {
            
            // Se acabou de conectar, assina os tópicos
            if (!estavaConectado) {
                estavaConectado = true;
                network.subscribe("chuveiro/limite");
            }

            // Consome fila de envio (Publish)
            if (xQueueReceive(filaMQTT, &msg, pdMS_TO_TICKS(10)) == pdTRUE) {
                 network.publish(msg.topico, msg.payload);
            }
        } else {
            estavaConectado = false;
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ============================================================
// TASK DE SENSOR
// ============================================================
void taskSensor(void * parameter) {
    const unsigned long INTERVALO_ACUMULO = 5000; // 5 segundos
    float acumulado = 0.0;
    MensagemMQTT msg;

    for(;;) {
        // Coleta e zera pulsos (Atômico)
        unsigned long pulsos = sensorChuveiro.getPulsosAndReset();
        
        if (pulsos > 0) {
            acumulado += (pulsos / 450.0);
        }

        // Aguarda 5 segundos acumulando
        vTaskDelay(pdMS_TO_TICKS(INTERVALO_ACUMULO));

        // Se houve fluxo neste intervalo, enfileira
        if (acumulado > 0) {
            strcpy(msg.topico, "chuveiro/telemetria");
            snprintf(msg.payload, 20, "%.3f", acumulado);
            
            // Envia para a fila (timeout grande para garantir entrega interna)
            if (xQueueSend(filaMQTT, &msg, pdMS_TO_TICKS(100)) != pdTRUE) {
                Serial.println("[Sensor] Erro: Fila cheia!");
            }
            
            acumulado = 0.0; // Zera para o próximo ciclo
        }
    }
}

// ============================================================
// SETUP & LOOP
// ============================================================
void setup() {
    Serial.begin(115200);

    // Inicializa Hardware
    ledAviso.begin();
    buzzerAlerta.begin();
    
    // Teste de Boot (Bip + Piscada)
    ledAviso.ligar();
    buzzerAlerta.bip(100);
    delay(200);
    ledAviso.desligar();

    // RTOS
    filaMQTT = xQueueCreate(20, sizeof(MensagemMQTT)); 
    semaforoFluxo = xSemaphoreCreateBinary();

    sensorChuveiro.begin(semaforoFluxo);

    xTaskCreatePinnedToCore(taskRede, "NetTask", 4096, NULL, 1, &handleTaskRede, 0);
    xTaskCreatePinnedToCore(taskSensor, "SensorTask", 4096, NULL, 1, &handleTaskSensor, 1);
    
    vTaskSuspend(handleTaskRede);
    vTaskSuspend(handleTaskSensor);
    
    Serial.println(">>> SISTEMA PRONTO (SLEEP) <<<");
}

void loop() {
    if (xSemaphoreTake(semaforoFluxo, portMAX_DELAY) == pdTRUE) {
        Serial.println(">>> WAKE UP! Banho Detectado <<<");
        vTaskResume(handleTaskRede);
        vTaskResume(handleTaskSensor);
        
        // Envia INICIO
        MensagemMQTT msgInicio;
        strcpy(msgInicio.topico, "chuveiro/status");
        strcpy(msgInicio.payload, "INICIO");
        xQueueSend(filaMQTT, &msgInicio, 0);

        // Monitoramento
        unsigned long tempoSemPulsos = 0;
        const unsigned long TIMEOUT_SLEEP = 10000; 

        while (tempoSemPulsos < TIMEOUT_SLEEP) {
            if (xSemaphoreTake(semaforoFluxo, pdMS_TO_TICKS(100)) == pdTRUE) {
                tempoSemPulsos = 0; 
            } else {
                tempoSemPulsos += 100; 
            }
        }

        Serial.println(">>> FIM DE FLUXO. <<<");
        
        // Envia FIM
        MensagemMQTT msgFim;
        strcpy(msgFim.topico, "chuveiro/status");
        strcpy(msgFim.payload, "FIM");
        xQueueSend(filaMQTT, &msgFim, 0);
        
        // Garante que o Buzzer desliga ao fim do banho (segurança)
        buzzerAlerta.parar();
        buzzerAtivo = false;

        vTaskDelay(pdMS_TO_TICKS(1000));

        // Esvazia fila
        int drain = 0;
        while (uxQueueMessagesWaiting(filaMQTT) > 0 && drain < 50) {
            vTaskDelay(pdMS_TO_TICKS(100));
            drain++;
        }

        vTaskSuspend(handleTaskSensor);
        vTaskSuspend(handleTaskRede);
        xSemaphoreTake(semaforoFluxo, 0); // Limpa semáforo
    }
}