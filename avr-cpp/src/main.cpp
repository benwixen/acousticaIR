#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "infra.h"
#include "usart.h"
#include "avr_utils.h"

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

constexpr uint8_t POWER_TOGGLE_PIN { PD6 };
constexpr uint8_t VOLUME_UP_PIN { PD5 };
constexpr uint8_t VOLUME_DOWN_PIN { PD4 };
constexpr uint8_t AUDIO_MODE_PIN { PD3 };

char string_buffer[20];
uint32_t received_value { 0 };

#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    infra::initInfra();
    avr::setPinToOutput(DDRD, POWER_TOGGLE_PIN);
    avr::setPinToOutput(DDRD, VOLUME_UP_PIN);
    avr::setPinToOutput(DDRD, VOLUME_DOWN_PIN);
    avr::setPinToOutput(DDRD, AUDIO_MODE_PIN);

    usart::initUsart();
    usart::printString("Ready to receive IR signals at PD2/INT0.\n");

    while (true) {
        received_value = infra::checkForValue();
        if (received_value != 0) {
            sprintf(string_buffer, "Data=%lu\n", received_value);
            usart::printString(string_buffer);
            if (received_value == 3'772'790'473) {
                usart::printString("Power\n");
                avr::setOutputPinHigh(DDRD, POWER_TOGGLE_PIN);
                _delay_ms(500);
                avr::setOutputPinLow(DDRD, POWER_TOGGLE_PIN);
                _delay_ms(1000);
            } else if (received_value == 3'772'833'823) {
                usart::printString("Volume up\n");
                avr::setOutputPinHigh(DDRD, VOLUME_UP_PIN);
                _delay_ms(250);
                avr::setOutputPinLow(DDRD, VOLUME_UP_PIN);
            } else if (received_value == 3'772'829'743) {
                usart::printString("Volume down\n");
                avr::setOutputPinHigh(DDRD, VOLUME_DOWN_PIN);
                _delay_ms(250);
                avr::setOutputPinLow(DDRD, VOLUME_DOWN_PIN);
            } else if (received_value == 3'772'803'223) {
                usart::printString("Mode\n");
                avr::setOutputPinHigh(DDRD, AUDIO_MODE_PIN);
                _delay_ms(500);
                avr::setOutputPinLow(DDRD, AUDIO_MODE_PIN);
                _delay_ms(1000);
            }

            infra::resume();
        }
        _delay_ms(100);
    }
}
