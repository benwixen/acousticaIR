#include "infra.h"
#include "stdint.h"

namespace {
    inline void clearBit(volatile uint32_t& dword, uint8_t index) {
        dword &= ~(1UL << index);
    }

    inline void setBit(volatile uint32_t& dword, uint8_t index) {
        dword |= (1UL << index);
    }
}

namespace infra {
    static const uint8_t LEADING_MARKS = 2;
    static const uint8_t NUMBER_OF_BITS = 32;
    static const uint8_t NUMBER_OF_MARKS = LEADING_MARKS + (NUMBER_OF_BITS * 2);

    inline bool isShortMark(uint32_t difference) {
        return difference > 450 && difference < 650;
    }

    inline bool isMediumMark(uint32_t difference) {
        return difference > 1500 && difference < 1700;
    }

    inline bool isLongMark(uint32_t difference) {
        return difference > 4400 && difference < 4600;
    }

    enum class MatchResult : uint8_t {
        ByteZero,
        ByteOne,
        SkipAhead,
        Reset
    };

    inline MatchResult matchDifference(uint8_t mark_counter, uint32_t difference) {
        if (mark_counter < LEADING_MARKS) {
            return isLongMark(difference) ? MatchResult::SkipAhead : MatchResult::Reset;
        } else if (mark_counter % 2 == 0) {
            return isShortMark(difference) ? MatchResult::SkipAhead : MatchResult::Reset;
        } else if (isShortMark(difference)) {
            return MatchResult::ByteZero;
        } else if (isMediumMark(difference)) {
            return MatchResult::ByteOne;
        } else {
            return MatchResult::Reset;
        }
    }

    inline uint8_t bitIndex(uint8_t mark_counter) {
        return NUMBER_OF_BITS - (mark_counter / 2);
    }

    inline bool didReachEnd(uint8_t mark_counter) {
        return mark_counter == NUMBER_OF_MARKS;
    }

    volatile uint32_t data;
    volatile uint8_t mark_counter { 0 };
    volatile uint32_t last_time { 0 };
    volatile uint32_t current_time;
    volatile uint32_t diff;

    void listenToIrReceivePin() {
        if (didReachEnd(mark_counter)) return;

        current_time = micros();
        diff = current_time - last_time;
        last_time = current_time;

        auto result = matchDifference(mark_counter, diff);
        switch (result) {
            case MatchResult::SkipAhead:
                mark_counter++;
                break;
            case MatchResult::ByteZero:
                ::clearBit(data, bitIndex(mark_counter));
                mark_counter++;
                break;
            case MatchResult::ByteOne:
                ::setBit(data, bitIndex(mark_counter));
                mark_counter++;
                break;
            case MatchResult::Reset:
                mark_counter = 0;
                break;
        }
    }

    void initInfra(uint8_t receive_pin) {
        attachInterrupt(digitalPinToInterrupt(receive_pin), listenToIrReceivePin, CHANGE);
    }

    uint32_t checkForValue() {
        if (didReachEnd(mark_counter)) {
            return data;
        } else {
            return 0;
        }
    }

    void resume() {
        infra::mark_counter = 0;
    }
}
