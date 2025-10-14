#include <Wire.h>
#include <Arduino.h>

// Adresse I2C de l'esclave. DOIT correspondre à l'adresse de l'esclave.
#define I2C_DEV_ADDR 0x55

// Compteur pour les données envoyées
uint32_t i = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 I2C Master");
  
  // Initialisation du bus I2C en mode Maître
  // Utilise les broches par défaut : SDA=GPIO21, SCL=GPIO22
  Wire.begin(); 
}

void loop() {
  delay(5000); // Attendre 5 secondes entre les cycles

  // --- ÉTAPE 1: ÉCRIRE (Envoyer un message à l'esclave) ---
  
  Serial.print("Sending to Slave: ");
  
  // Démarrer la transmission vers l'adresse 0x55
  Wire.beginTransmission(I2C_DEV_ADDR);
  
  // Écrire les données dans le tampon de transmission
  Wire.printf("Hello World! %lu", i++); 
  
  // Terminer la transmission
  uint8_t error = Wire.endTransmission(true);
  
  if (error == 0) {
    Serial.println("Success");
  } else {
    Serial.printf("Error endTransmission: %u\n", error);
  }

  // --- ÉTAPE 2: LIRE (Demander des données à l'esclave) ---

  // Demander 16 octets à l'esclave
  uint8_t bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, 16); 

  Serial.printf("Request from Slave: %u bytes received\n", bytesReceived);

  if (bytesReceived > 0) {
    Serial.print("Data received: ");
    // Lire les octets reçus
    while (Wire.available()) {
      Serial.write(Wire.read());
    }
    Serial.println();
  }
}