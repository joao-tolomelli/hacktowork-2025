#include "NetworkManager.h"

NetworkManager::NetworkManager(const char* ssid, const char* password, const char* mqttServer, int mqttPort) 
    : _mqttClient(_espClient) {
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
    if (WiFi.status() == WL_CONNECTED) return;

    Serial.print("[Network] Conectando ao Wi-Fi: ");
    Serial.println(_ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);
    
    // CORREÇÃO: Timeout para não ficar preso aqui para sempre (Watchdog Friendly)
    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
        delay(500);
        Serial.print(".");
        tentativas++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[Network] Wi-Fi Conectado!");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n[Network] Falha ao conectar no Wi-Fi.");
    }
}

void NetworkManager::reconnectMqtt() {
    if (!_mqttClient.connected()) {
        // Verifica se tem wifi antes de tentar mqtt
        if (WiFi.status() != WL_CONNECTED) return;

        Serial.print("[Network] Tentando MQTT... ");
        String clientId = "ESP32-" + String(random(0xffff), HEX);
        
        if (_mqttClient.connect(clientId.c_str())) {
            Serial.println("Conectado!");
        } else {
            // Falha silenciosa para não travar o loop
            Serial.print("Falha rc=");
            Serial.print(_mqttClient.state());
            Serial.println("");
        }
    }
}

void NetworkManager::update() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    
    // Só tenta MQTT se tiver Wi-Fi
    if (WiFi.status() == WL_CONNECTED && !_mqttClient.connected()) {
        reconnectMqtt();
    }
    
    if (_mqttClient.connected()) {
        _mqttClient.loop();
    }
}

void NetworkManager::publish(const char* topic, const char* payload) {
    if (_mqttClient.connected()) {
        _mqttClient.publish(topic, payload);
    }
}

bool NetworkManager::isConnected() {
    return _mqttClient.connected();
}

void NetworkManager::setCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
    _mqttClient.setCallback(callback);
}

void NetworkManager::subscribe(const char* topic) {
    if (_mqttClient.connected()) {
        _mqttClient.subscribe(topic);
        Serial.print("[Network] Inscrito no tópico: ");
        Serial.println(topic);
    }
}