#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DHT.h"

// --- Définir les broches ---
#define LED_PIN 25          // LED FireBeetle
#define DHT_PIN 26          // ⚡ Branche ton DHT sur GPIO4 par ex.
#define DHT_TYPE DHT22     // ou DHT11 si tu as ce modèle

// Initialisation du capteur
DHT dht(DHT_PIN, DHT_TYPE);

// --- Tâche 1 : clignotement LED toutes les 2s ---
void vTaskBlinkLED(void *pvParameters)
{
  pinMode(LED_PIN, OUTPUT);

  for (;;)
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.printf("LED ON (core %d)\n", xPortGetCoreID());
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    digitalWrite(LED_PIN, LOW);
    Serial.printf("LED OFF (core %d)\n", xPortGetCoreID());
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// --- Tâche 2 : lecture température toutes les 5s ---
void vTaskReadTemperature(void *pvParameters)
{
  for (;;)
  {
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    if (isnan(temp) || isnan(hum))
    {
      Serial.println("Erreur lecture capteur DHT !");
    }
    else
    {
      Serial.printf("Température : %.1f °C | Humidité : %.1f %% (core %d)\n", 
                     temp, hum, xPortGetCoreID());
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  dht.begin();

  // Créer la tâche LED sur core 0
  xTaskCreatePinnedToCore(vTaskBlinkLED, "TaskBlinkLED", 4096, NULL, 1, NULL, 0);

  // Créer la tâche Température sur core 1
  xTaskCreatePinnedToCore(vTaskReadTemperature, "TaskReadTemp", 4096, NULL, 1, NULL, 1);
}

void loop()
{
  // Boucle Arduino principale → tâche de fond
  Serial.printf("loop() tourne sur le core %d\n", xPortGetCoreID());
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
