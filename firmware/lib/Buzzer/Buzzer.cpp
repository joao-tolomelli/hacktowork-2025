#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pino, uint8_t canal) {
    _pino = pino;
    _canalPWM = canal;
    _ativo = false;
}

void Buzzer::begin() {
    pinMode(_pino, OUTPUT);
    digitalWrite(_pino, HIGH); // Inicia desligado (Transistor PNP desliga com HIGH)

    // Configuração do PWM (LEDC)
    // Frequência base 2000Hz, resolução 8 bits
    ledcSetup(_canalPWM, 2000, 8);
    ledcAttachPin(_pino, _canalPWM);
}

void Buzzer::tocar(unsigned int frequencia) {
    if (!_ativo) {
        // Toca o tom no canal definido
        ledcWriteTone(_canalPWM, frequencia);
        _ativo = true;
    }
}

void Buzzer::parar() {
    if (_ativo) {
        ledcWriteTone(_canalPWM, 0); // Frequência 0 desliga
        digitalWrite(_pino, HIGH);   // Garante nível ALTO para desligar o transistor PNP
        _ativo = false;
    }
}

void Buzzer::bip(unsigned int duracaoMs) {
    tocar(2500); // 2.5kHz
    delay(duracaoMs);
    parar();
}