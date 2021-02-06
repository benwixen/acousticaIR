#ifndef AVRUTILS_H
#define AVRUTILS_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

inline void avr_setBit(volatile uint8_t* reg, uint8_t bit) {
    *reg |= (1 << bit);
}

inline void avr_clearBit(volatile uint8_t* reg, uint8_t bit) {
    *reg &= ~(1 << bit);
}

inline void avr_setPinToOutput(volatile uint8_t* ddr, uint8_t pin) {
    avr_setBit(ddr, pin);
}

inline void avr_setOutputPinHigh(volatile uint8_t* ddr, uint8_t pin) {
    avr_setBit(ddr, pin);
}

inline void avr_setOutputPinLow(volatile uint8_t* ddr, uint8_t pin) {
    avr_clearBit(ddr, pin);
}

inline void avr_setTimer1ToPrescaleBy256() {
    TCCR1B |= (1 << CS12);
}

inline uint16_t avr_getTimer1Count() {
    return TCNT1;
}

inline void avr_configureInterruptsForINT0() {
    EIMSK |= (1 << INT0);
}

inline void avr_triggerInterruptsOnVoltageChanges() {
    EICRA |= (1 << ISC00);
}

inline void avr_enableInterrupts() {
    sei();
}

#endif //AVR_UTILS_H
