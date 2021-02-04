#include "arduino_mock.h"

volatile uint8_t TCCR2A;
volatile uint8_t TCCR2B;
volatile uint8_t OCR2A;
volatile uint8_t OCIE2A;
volatile uint8_t TCNT2;
volatile uint8_t WGM21;
volatile uint8_t CS21;
volatile uint8_t TIMSK2;
volatile uint8_t PORTB;

volatile uint8_t NEXT_READ;
volatile uint32_t NEXT_MICROS;

void delay(uint16_t ms) {}

void noInterrupts() {}

void interrupts() {}

void pinMode(uint8_t pin, uint8_t mode) {}

uint8_t digitalPinToInterrupt(uint8_t pin) {
    return 0;
}

void attachInterrupt(uint8_t interrupt, void (*pin)(), int i) {}

uint8_t digitalRead(uint8_t pin) {
    return NEXT_READ;
}

uint32_t micros() {
    return NEXT_MICROS;
}

void digitalWrite(uint8_t pin, uint8_t value) {}