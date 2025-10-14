#include <Wire.h>
#include <Arduino.h>
// Adresse I2C de l'esclave. DOIT correspondre à l'adresse du Maître.
#define I2C_SLAVE_ADDR 0x55

// Variable partagée pour les données
uint32_t j = 0;

// Fonction de rappel appelée lorsque le Maître envoie des données
void onReceive(int len) {
  Serial.printf("onReceive[%d]: ", len);
  
  // Lire toutes les données du Maître
  while (Wire.available()) {
    Serial.write(Wire.read());
  }
  Serial.println();
}

// Fonction de rappel appelée lorsque le Maître demande des données
void onRequest() {
  // Préparer les données à envoyer au Maître
  Wire.printf("%lu Packets.", j++);
  Serial.println("onRequest: Data sent");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 I2C Slave");
  
  // Initialisation du bus I2C en mode Esclave avec l'adresse 0x55
  // Utilise les broches par défaut : SDA=GPIO21, SCL=GPIO22
  Wire.begin(I2C_SLAVE_ADDR); 
  
  // Attacher la fonction onReceive pour la réception de données
  Wire.onReceive(onReceive);
  
  // Attacher la fonction onRequest pour la demande de données
  Wire.onRequest(onRequest);
}

void loop() {
  // La logique I2C est gérée par les fonctions de rappel (interruptions)
  // La boucle loop() peut être utilisée pour d'autres tâches
  delay(1000); 
}