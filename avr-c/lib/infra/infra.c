#include "infra.h"

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "avr_utils.h"

inline void setBitOfDword(volatile uint32_t* dword, uint8_t index) {
    *dword |= (1UL << index);
}

inline void clearBitOfDword(volatile uint32_t* dword, uint8_t index) {
    *dword &= ~(1UL << index);
}

#define LEADING_MARKS 2
#define NUMBER_OF_BITS 32
#define NUMBER_OF_MARKS (LEADING_MARKS + (NUMBER_OF_BITS * 2))

inline bool isShortMark(uint32_t difference) {
    return difference > 450 && difference < 650;
}

inline bool isMediumMark(uint32_t difference) {
    return difference > 1500 && difference < 1700;
}

inline bool isLongMark(uint32_t difference) {
    return difference > 4400 && difference < 4600;
}

enum MatchResult {
    ByteZero,
    ByteOne,
    SkipAhead,
    Reset
};

inline enum MatchResult matchDifference(uint8_t mark_counter, uint32_t difference) {
    if (mark_counter < LEADING_MARKS) {
        return isLongMark(difference) ? SkipAhead : Reset;
    } else if (mark_counter % 2 == 0) {
        return isShortMark(difference) ? SkipAhead : Reset;
    } else if (isShortMark(difference)) {
        return ByteZero;
    } else if (isMediumMark(difference)) {
        return ByteOne;
    } else {
        return Reset;
    }
}

inline uint8_t bitIndex(uint8_t mark_counter) {
    return NUMBER_OF_BITS - (mark_counter / 2);
}

inline bool didReachEnd(uint8_t mark_counter) {
    return mark_counter == NUMBER_OF_MARKS;
}

volatile uint32_t data;
volatile uint8_t mark_counter = 0;
volatile uint32_t last_time = 0;
volatile uint32_t diff_in_us;
volatile uint16_t current_time;

ISR(INT0_vect) {
    if (didReachEnd(mark_counter)) return;

    current_time = avr_getTimer1Count(); // timer 1 value
    diff_in_us = (current_time - last_time) * 16;
    last_time = current_time;

    enum MatchResult result = matchDifference(mark_counter, diff_in_us);
    switch (result) {
        case SkipAhead:
            mark_counter++;
            break;
        case ByteZero:
            clearBitOfDword(&data, bitIndex(mark_counter));
            mark_counter++;
            break;
        case ByteOne:
            setBitOfDword(&data, bitIndex(mark_counter));
            mark_counter++;
            break;
        case Reset:
        default:
            mark_counter = 0;
            break;
    }
}

void infra_init() {
    avr_setTimer1ToPrescaleBy256(); // 64us per tick

    avr_configureInterruptsForINT0();
    avr_triggerInterruptsOnVoltageChanges();
    avr_enableInterrupts();
}

uint32_t infra_checkForValue() {
    if (didReachEnd(mark_counter)) {
        return data;
    } else {
        return 0;
    }
}

void infra_resume() {
    mark_counter = 0;
}

