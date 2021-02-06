#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

#include "infra.h"
#include "usart.h"
#include "avr_utils.h"

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

static const uint8_t POWER_TOGGLE_PIN = PD6;
static const uint8_t VOLUME_UP_PIN = PD5;
static const uint8_t VOLUME_DOWN_PIN = PD4;
static const uint8_t AUDIO_MODE_PIN = PD3;

char string_buffer[20];
uint32_t received_value = 0;

#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    infra_init();
    avr_setPinToOutput(&DDRD, POWER_TOGGLE_PIN);
    avr_setPinToOutput(&DDRD, VOLUME_UP_PIN);
    avr_setPinToOutput(&DDRD, VOLUME_DOWN_PIN);
    avr_setPinToOutput(&DDRD, AUDIO_MODE_PIN);

    usart_init();
    usart_printString("Ready to receive IR signals at PD2/INT0.\n");

    while (true) {
        received_value = infra_checkForValue();
        if (received_value != 0) {
            sprintf(string_buffer, "Data=%lu\n", received_value);
            usart_printString(string_buffer);
            if (received_value == 3772790473) {
                usart_printString("Power\n");
                avr_setOutputPinHigh(&DDRD, POWER_TOGGLE_PIN);
                _delay_ms(500);
                avr_setOutputPinLow(&DDRD, POWER_TOGGLE_PIN);
                _delay_ms(1000);
            } else if (received_value == 3772833823) {
                usart_printString("Volume up\n");
                avr_setOutputPinHigh(&DDRD, VOLUME_UP_PIN);
                _delay_ms(250);
                avr_setOutputPinLow(&DDRD, VOLUME_UP_PIN);
            } else if (received_value == 3772829743) {
                usart_printString("Volume down\n");
                avr_setOutputPinHigh(&DDRD, VOLUME_DOWN_PIN);
                _delay_ms(250);
                avr_setOutputPinLow(&DDRD, VOLUME_DOWN_PIN);
            } else if (received_value == 3772803223) {
                usart_printString("Mode\n");
                avr_setOutputPinHigh(&DDRD, AUDIO_MODE_PIN);
                _delay_ms(500);
                avr_setOutputPinLow(&DDRD, AUDIO_MODE_PIN);
                _delay_ms(1000);
            }

            infra_resume();
        }
        _delay_ms(100);
    }
}
