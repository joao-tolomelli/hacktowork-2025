#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led {
private:
    uint8_t _pino;
    bool _ativo; // Para controlar estado lógico (High/Low) dependendo do hardware

public:
    // Construtor
    Led(uint8_t pino);

    void begin();
    void ligar();
    void desligar();
    bool estaLigado();
};

#endif