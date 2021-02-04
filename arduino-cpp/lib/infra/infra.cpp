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

    inline MatchResult matchDifference(uint8_t markCounter, uint32_t difference) {
        if (markCounter < LEADING_MARKS) {
            return isLongMark(difference) ? MatchResult::SkipAhead : MatchResult::Reset;
        } else if (markCounter % 2 == 0) {
            return isShortMark(difference) ? MatchResult::SkipAhead : MatchResult::Reset;
        } else if (isShortMark(difference)) {
            return MatchResult::ByteZero;
        } else if (isMediumMark(difference)) {
            return MatchResult::ByteOne;
        } else {
            return MatchResult::Reset;
        }
    }

    inline uint8_t bitIndex(uint8_t markCounter) {
        return NUMBER_OF_BITS - (markCounter / 2);
    }

    inline bool didReachEnd(uint8_t markCounter) {
        return markCounter == NUMBER_OF_MARKS;
    }

    volatile uint32_t data;
    volatile uint8_t markCounter { 0 };
    volatile uint32_t lastTime { 0 };
    volatile uint32_t currentTime;
    volatile uint32_t diff;

    void listenToIrReceivePin() {
        if (didReachEnd(markCounter)) return;

        currentTime = micros();
        diff = currentTime - lastTime;
        lastTime = currentTime;

        auto result = matchDifference(markCounter, diff);
        switch (result) {
            case MatchResult::SkipAhead:
                markCounter++;
                break;
            case MatchResult::ByteZero:
                ::clearBit(data, bitIndex(markCounter));
                markCounter++;
                break;
            case MatchResult::ByteOne:
                ::setBit(data, bitIndex(markCounter));
                markCounter++;
                break;
            case MatchResult::Reset:
                markCounter = 0;
                break;
        }
    }

    void initInfra(uint8_t receivePin) {
        attachInterrupt(digitalPinToInterrupt(receivePin), listenToIrReceivePin, CHANGE);
    }

    uint32_t checkForValue() {
        if (didReachEnd(markCounter)) {
            return data;
        } else {
            return 0;
        }
    }

    void resume() {
        infra::markCounter = 0;
    }
}
