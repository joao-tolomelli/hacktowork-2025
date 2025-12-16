#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class NetworkManager {
private:
    const char* _ssid;
    const char* _password;
    const char* _mqttServer;
    int _mqttPort;

    WiFiClient _espClient;
    PubSubClient _mqttClient;

    void connectWifi();
    void reconnectMqtt();

public:
    // Construtor: Recebe as credenciais
    NetworkManager(const char* ssid, const char* password, const char* mqttServer, int mqttPort = 1883);

    // Inicializa Wi-Fi e define servidor MQTT
    void begin();

    // Mantém a conexão viva (deve ser chamado no loop/task)
    void update();

    // Publica mensagens
    void publish(const char* topic, const char* payload);
    
    // Verifica se está conectado
    bool isConnected();
};

#endif