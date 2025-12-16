#include "FlowSensor.h"

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Inicialização das variáveis estáticas
volatile unsigned long FlowSensor::_totalPulsos = 0;

FlowSensor::FlowSensor(uint8_t pino, float fator) {
    _pino = pino;
    _fatorCalibracao = fator;
}

// A Interrupção (ISR) precisa ser muito rápida
void IRAM_ATTR FlowSensor::_isr() {
    _totalPulsos++;
}

void FlowSensor::begin() {
    pinMode(_pino, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pino), _isr, RISING);
}

float FlowSensor::getVolume() {
    unsigned long pulsosSeguros;
    
    // CRITICAL SECTION: Pausa interrupções momentaneamente para ler a variável
    // Isso evita "corrupção" de dados em arquiteturas dual-core como ESP32
    portENTER_CRITICAL_ISR(&mux);
    pulsosSeguros = _totalPulsos;
    portEXIT_CRITICAL_ISR(&mux);
    
    return pulsosSeguros / _fatorCalibracao;
}

unsigned long FlowSensor::getPulsosRaw() {
    // Retorna pulsos crus para compararmos se houve mudança
    return _totalPulsos;
}

void FlowSensor::reset() {
    portENTER_CRITICAL_ISR(&mux);
    _totalPulsos = 0;
    portEXIT_CRITICAL_ISR(&mux);
}