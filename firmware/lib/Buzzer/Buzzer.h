#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
private:
    uint8_t _pino;
    uint8_t _canalPWM; // O ESP32 tem canais de 0 a 15
    bool _ativo;

public:
    Buzzer(uint8_t pino, uint8_t canal = 0);
    
    void begin();
    
    // Toca um tom contínuo (frequência em Hz)
    void tocar(unsigned int frequencia = 2000);
    
    // Para o som
    void parar();
    
    // Faz um "Bip" curto (bloqueante ou não, aqui faremos simples)
    void bip(unsigned int duracaoMs);
};

#endif