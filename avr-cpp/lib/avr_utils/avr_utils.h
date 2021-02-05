#ifndef AVRUTILS_H
#define AVRUTILS_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

namespace {
    inline void setBit(volatile uint8_t& reg, uint8_t bit) {
        reg |= (1 << bit);
    }

    inline void clearBit(volatile uint8_t& reg, uint8_t bit) {
        reg &= ~(1 << bit);
    }
}

namespace avr {
    inline void setPinToOutput(volatile uint8_t& ddr, uint8_t pin) {
        setBit(ddr, pin);
    }

    inline void setOutputPinHigh(volatile uint8_t& ddr, uint8_t pin) {
        setBit(ddr, pin);
    }

    inline void setOutputPinLow(volatile uint8_t& ddr, uint8_t pin) {
        clearBit(ddr, pin);
    }

    inline void setTimer1ToPrescaleBy256() {
        TCCR1B |= (1 << CS12);
    }

    inline uint16_t getTimer1Count() {
        return TCNT1;
    }

    inline void configureInterruptsForINT0() {
        EIMSK |= (1 << INT0);
    }

    inline void triggerInterruptsOnVoltageChanges() {
        EICRA |= (1 << ISC00);
    }

    inline void enableInterrupts() {
        sei();
    }
}

#endif //AVR_UTILS_H
