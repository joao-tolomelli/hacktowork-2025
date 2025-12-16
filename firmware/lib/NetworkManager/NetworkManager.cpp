#include "NetworkManager.h"

NetworkManager::NetworkManager(const char* ssid, const char* password, const char* mqttServer, int mqttPort) 
    : _mqttClient(_espClient) { // Inicializa o PubSubClient com o WiFiClient
    _ssid = ssid;
    _password = password;
    _mqttServer = mqttServer;
    _mqttPort = mqttPort;
}

void NetworkManager::begin() {
    connectWifi();
    _mqttClient.setServer(_mqttServer, _mqttPort);
}

void NetworkManager::connectWifi() {
    Serial.print("[Network] Conectando ao Wi-Fi: ");
    Serial.println(_ssid);
    
    WiFi.begin(_ssid, _password);
    
    // Pequeno loop de bloqueio apenas no boot
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[Network] Wi-Fi Conectado!");
    Serial.print("[Network] IP: ");
    Serial.println(WiFi.localIP());
}

void NetworkManager::reconnectMqtt() {
    // Tenta conectar se não estiver conectado
    if (!_mqttClient.connected()) {
        Serial.print("[Network] Tentando MQTT... ");
        
        // ID Randômico
        String clientId = "ESP32-" + String(random(0xffff), HEX);
        
        if (_mqttClient.connect(clientId.c_str())) {
            Serial.println("Conectado!");
        } else {
            Serial.print("Falha, rc=");
            Serial.print(_mqttClient.state());
        }
    }
}

void NetworkManager::update() {
    // Garante que o Wi-Fi está ok
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }

    // Garante MQTT
    if (!_mqttClient.connected()) {
        reconnectMqtt();
    }
    
    // Processa pacotes de entrada/keep-alive
    _mqttClient.loop();
}

void NetworkManager::publish(const char* topic, const char* payload) {
    if (_mqttClient.connected()) {
        _mqttClient.publish(topic, payload);
    }
}

bool NetworkManager::isConnected() {
    return _mqttClient.connected();
}