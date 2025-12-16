#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>

class FlowSensor {
private:
    uint8_t _pino;
    float _fatorCalibracao;
    
    // Variáveis estáticas para funcionarem dentro da interrupção
    static volatile unsigned long _totalPulsos;
    static void IRAM_ATTR _isr(); // A função de interrupção

public:
    // Construtor
    FlowSensor(uint8_t pino, float fator = 450.0);

    void begin();
    
    // Retorna o volume acumulado desde o último reset
    float getVolume();
    
    // Zera a contagem (início de um novo banho)
    void reset();
    
    // Retorna se há fluxo (pulsos mudando) para ajudar na lógica de timeout
    unsigned long getPulsosRaw();
};

#endif