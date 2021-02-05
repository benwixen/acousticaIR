#include "infra.h"

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

constexpr uint8_t POWER_TOGGLE_PIN { 6 };
constexpr uint8_t VOLUME_UP_PIN { 5 };
constexpr uint8_t VOLUME_DOWN_PIN { 4 };
constexpr uint8_t AUDIO_MODE_PIN { 3 };
constexpr uint8_t IR_RECEIVE_PIN { 2 };

void setup() {
    infra::initInfra(IR_RECEIVE_PIN);
    pinMode(POWER_TOGGLE_PIN, OUTPUT);
    pinMode(VOLUME_UP_PIN, OUTPUT);
    pinMode(VOLUME_DOWN_PIN, OUTPUT);
    pinMode(AUDIO_MODE_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
    digitalWrite(POWER_TOGGLE_PIN, LOW);
    digitalWrite(VOLUME_UP_PIN, LOW);
    digitalWrite(VOLUME_DOWN_PIN, LOW);
    digitalWrite(AUDIO_MODE_PIN, LOW);
}

uint32_t received_value { 0 };

void loop() {
    received_value = infra::checkForValue();
    if (received_value != 0) {
        Serial.print(F("Data="));
        Serial.print(received_value);
        Serial.println();
        if (received_value == 3'772'790'473) {
            Serial.println(F("Power"));
            digitalWrite(POWER_TOGGLE_PIN, HIGH);
            delay(500);
            digitalWrite(POWER_TOGGLE_PIN, LOW);
            delay(1000);
        } else if (received_value == 3'772'833'823) {
            Serial.println(F("Volume up"));
            digitalWrite(VOLUME_UP_PIN, HIGH);
            delay(250);
            digitalWrite(VOLUME_UP_PIN, LOW);
        } else if (received_value == 3'772'829'743) {
            Serial.println(F("Volume down"));
            digitalWrite(VOLUME_DOWN_PIN, HIGH);
            delay(250);
            digitalWrite(VOLUME_DOWN_PIN, LOW);
        } else if (received_value == 3'772'803'223) {
            Serial.println(F("Mode"));
            digitalWrite(AUDIO_MODE_PIN, HIGH);
            delay(500);
            digitalWrite(AUDIO_MODE_PIN, LOW);
            delay(1000);
        }

        infra::resume(); // Receive the next value
    }
    delay(100);
}
