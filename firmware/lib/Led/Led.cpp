#include "Led.h"

Led::Led(uint8_t pino) {
    _pino = pino;
    _ativo = false;
}

void Led::begin() {
    pinMode(_pino, OUTPUT);
    desligar(); // Inicia apagado
}

void Led::ligar() {
    digitalWrite(_pino, HIGH); // Assumindo LED ativo em HIGH
    _ativo = true;
}

void Led::desligar() {
    digitalWrite(_pino, LOW);
    _ativo = false;
}

bool Led::estaLigado() {
    return _ativo;
}