#ifndef SECRETS_H
#define SECRETS_H

/**
 * ARQUIVO DE EXEMPLO DE SEGREDOS
 * * Instruções:
 * 1. Duplique este arquivo e renomeie a cópia para "secrets.h"
 * 2. Preencha as informações abaixo com suas credenciais reais
 * 3. Certifique-se de que "secrets.h" está no seu .gitignore
 */

// --- Credenciais Wi-Fi ---
#define WIFI_SSID       "SEU_NOME_DE_REDE_AQUI"
#define WIFI_PASSWORD   "SUA_SENHA_WIFI_AQUI"

// --- Credenciais MQTT ---
#define MQTT_BROKER_IP  "192.168.1.100" // IP ou Domínio do Broker
#define MQTT_PORT       1883            // Porta padrão (1883 para não segura, 8883 para SSL)

#endif