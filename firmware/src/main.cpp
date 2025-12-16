#include <Arduino.h>
#include "FlowSensor.h"

// --- Objetos e Constantes ---
FlowSensor flowSensor(25, 450.0);

// --- Tasks do FreeRTOS ---
void taskFlowSensor(void *parameter)
{
  // Tempo limite até ele encerrar a sessão de banho (em ms)
  const unsigned long TIMEOUT_BANHO = 15000;

  unsigned long pulsosAnteriores = 0;
  unsigned long ultimaAtividade = 0;
  bool banhoAtivo = false;

  for (;;)
  {
    // Coleta dados
    unsigned long pulsosAtuais = flowSensor.getPulsosRaw();
    float volumeAtual = flowSensor.getVolume();

    // Detecção de Fluxo
    if (pulsosAtuais > pulsosAnteriores)
    {
      ultimaAtividade = millis();
      pulsosAnteriores = pulsosAtuais;

      if (!banhoAtivo)
      {
        banhoAtivo = true;
        flowSensor.reset();   // Zera hardware
        pulsosAnteriores = 0; // Zera controle local
        Serial.println(">>> SESSÃO INICIADA <<<");
      }
    }

    // Processamento de Estado
    if (banhoAtivo)
    {
      
      Serial.print("[Task] Volume: ");
      Serial.print(volumeAtual, 3);
      Serial.println(" L");

      // Verifica Timeout
      if (millis() - ultimaAtividade > TIMEOUT_BANHO)
      {
        Serial.println(">>> SESSÃO FINALIZADA (Timeout) <<<");
        Serial.print("Total Final: ");
        Serial.println(volumeAtual);

        banhoAtivo = false;
        flowSensor.reset();
        pulsosAnteriores = 0;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup()
{
  Serial.begin(115200);

  // Inicializa Hardware
  flowSensor.begin();

  // Cria as Tasks
  xTaskCreate(
      taskFlowSensor,       
      "FlowSensor",     
      4096,                    
      NULL,                    
      1,                       
      NULL 
  );

  Serial.println("Sistema Operacional Iniciado...");
}

void loop()
{
  // O loop fica vazio!
  // O FreeRTOS gerencia a taskFlowSensor em segundo plano.
  // Você pode deixar o loop para coisas triviais ou deletá-lo da mente.
  vTaskDelete(NULL); // Opcional: deleta a task "loop" para economizar RAM
}