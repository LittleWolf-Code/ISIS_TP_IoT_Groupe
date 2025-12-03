#include <Arduino.h>
#include "malekG-project-1_inferencing.h"
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

static float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
float previousMagnitude = 1.0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("=== Edge Impulse | Détection mouvement ===");

    Wire.begin();
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("ERREUR : MPU6050 non détecté !");
        while (1);
    }
    Serial.println("MPU6050 détecté !");
}

bool detectMovement() {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    float x = ax / 16384.0f;
    float y = ay / 16384.0f;
    float z = az / 16384.0f;

    float magnitude = sqrt(x*x + y*y + z*z);

    bool moved = fabs(magnitude - previousMagnitude) > 0.10; // Seuil de mouvement
    previousMagnitude = magnitude;

    return moved;
}

void loop() {

    // attendre un vrai mouvement
    if (!detectMovement()) {
        delay(20);
        return;
    }

    Serial.println("\nMouvement détecté → Inférence...");

    // remplir le buffer pour EI
    for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += 3) {
        int16_t ax, ay, az;
        mpu.getAcceleration(&ax, &ay, &az);

        features[i]   = ax / 16384.0f;
        features[i+1] = ay / 16384.0f;
        features[i+2] = az / 16384.0f;

        delay(1000 / EI_CLASSIFIER_FREQUENCY);
    }

    // conversion en signal
    signal_t signal;
    numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    // inférence
    ei_impulse_result_t result;
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

    if (res != EI_IMPULSE_OK) {
        Serial.println("Erreur d'inférence !");
        return;
    }

    // trouver la classe gagnante
    float bestValue = 0.0;
    const char* bestLabel = "";

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (result.classification[ix].value > bestValue) {
            bestValue = result.classification[ix].value;
            bestLabel = result.classification[ix].label;
        }
    }

    // afficher seulement la classe reconnue
    Serial.print("→ Mouvement détecté : ");
    Serial.println(bestLabel);
}
