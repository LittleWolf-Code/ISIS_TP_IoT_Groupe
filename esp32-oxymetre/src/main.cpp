#include <Wire.h>
#include "DFRobot_BloodOxygen_S.h"

#define I2C_ADDRESS 0x57  // exemple adresse I2C

// instanciation I2C
DFRobot_BloodOxygen_S_I2C hrSensor(&Wire, I2C_ADDRESS);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA=21, SCL=22
  Serial.println("Initialisation capteur…");
  if (!hrSensor.begin()) {
    Serial.println("Erreur initialisation capteur !");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("Capteur prêt !");
  hrSensor.sensorStartCollect();  // démarrer la collecte selon l’exemple
}

void loop() {
  hrSensor.getHeartbeatSPO2();
  
  int spo2 = hrSensor._sHeartbeatSPO2.SPO2; //Saturation pulsée en oxygène
  int bpm  = hrSensor._sHeartbeatSPO2.Heartbeat; 
  float tempC = hrSensor.getTemperature_C();

  Serial.print("SpO2: "); Serial.print(spo2); Serial.print("%  ");
  Serial.print("Heart Rate: "); Serial.print(bpm); Serial.print(" Beats Per Minute  ");
  Serial.print("Temp: "); Serial.print(tempC); Serial.println(" °C");

  delay(1000);
}