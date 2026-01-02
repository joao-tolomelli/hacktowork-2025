#include "FlowSensor.h"

// Inicialização das variáveis estáticas da classe
volatile unsigned long FlowSensor::_totalPulsos = 0;
SemaphoreHandle_t FlowSensor::_semaphoWakeUp = NULL;

// --- CORREÇÃO: Declaração do Mutex (Spinlock) ---
// É necessário instanciar o objeto que controla a seção crítica
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

FlowSensor::FlowSensor(uint8_t pino, float fator) {
    _pino = pino;
    _fatorCalibracao = fator;
}

// Interrupção (Roda na RAM)
void IRAM_ATTR FlowSensor::_isr() {
    // Protege a escrita na variável (opcional aqui pois é operação atômica simples, 
    // mas boa prática se a lógica crescer)
    portENTER_CRITICAL_ISR(&mux);
    _totalPulsos++;
    portEXIT_CRITICAL_ISR(&mux);
    
    // Libera o semáforo para acordar a task principal
    if (_semaphoWakeUp != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(_semaphoWakeUp, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}

void FlowSensor::begin(SemaphoreHandle_t semaforo) {
    _semaphoWakeUp = semaforo;
    pinMode(_pino, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pino), _isr, RISING);
}

unsigned long FlowSensor::getPulsosAndReset() {
    unsigned long pulsos;
    
    // ZONA CRÍTICA: Desabilita interrupções momentaneamente e trava o spinlock
    portENTER_CRITICAL(&mux); // No loop principal usa-se portENTER_CRITICAL (sem ISR)
    pulsos = _totalPulsos;
    _totalPulsos = 0; 
    portEXIT_CRITICAL(&mux);
    
    return pulsos;
}