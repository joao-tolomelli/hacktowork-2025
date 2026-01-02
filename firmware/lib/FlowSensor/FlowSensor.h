#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>
#include <freertos/semphr.h> // Necessário para o semáforo

class FlowSensor {
private:
    uint8_t _pino;
    float _fatorCalibracao;
    
    // Ponteiro para o semáforo que acorda o sistema
    static SemaphoreHandle_t _semaphoWakeUp;
    static volatile unsigned long _totalPulsos;
    static void IRAM_ATTR _isr(); 

public:
    FlowSensor(uint8_t pino, float fator = 450.0);
    
    // Agora aceita um semáforo opcional na inicialização
    void begin(SemaphoreHandle_t semaforo = NULL);
    
    // Retorna pulsos e ZERA o contador (para lógica de Delta)
    unsigned long getPulsosAndReset();
};

#endif