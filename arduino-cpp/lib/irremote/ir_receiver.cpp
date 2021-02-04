#include "ir_receiver.h"

IrReceiver::IrReceiver(uint8_t recvpin) {
    irparams.recvpin = recvpin;
    irparams.blinkflag = 0;
}

bool IrReceiver::decode() {
    if (irparams.rcvstate != ReceiveState::STOP) {
        return false;
    }

    /*
     * First copy 3 values from irparams to internal results structure
     */
    results.rawbuf = irparams.rawbuf;

    if (decodeSamsung()) {
        return true;
    }

    // Throw away and start over
    resume();
    return false;
}

//+=============================================================================
// initialization
//
void IrReceiver::enableIRIn() {
// the interrupt Service Routine fires every 50 uS
    noInterrupts();
    // Setup pulse clock timer interrupt
    // Prescale /8 (16M/8 = 0.5 microseconds per tick)
    // Therefore, the timer interval can range from 0.5 to 128 microseconds
    // Depending on the reset value (255 to 0)

    // The top value for the timer.  The modulation frequency will be F_CPU / 2 / OCR2A.
    const uint64_t TIMER_COUNT_TOP { F_CPU * MICROS_PER_TICK / 1'000'000 };

    /*
     * It generates an interrupt each 50 (MICROS_PER_TICK) us.
     */
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS21);
    OCR2A = TIMER_COUNT_TOP / 8;
    TCNT2 = 0;

    // Timer2 Overflow Interrupt Enable
    (TIMSK2 = _BV(OCIE2A));  // Output Compare Match A Interrupt Enable

    interrupts();

    // Initialize state machine state
    irparams.rcvstate = ReceiveState::IDLE;

    // Set pin modes
    pinMode(irparams.recvpin, INPUT);
}

//+=============================================================================
// Enable/disable blinking of pin 13 on IR processing
//
void IrReceiver::blink13(int blinkflag) {
    irparams.blinkflag = blinkflag;
    if (blinkflag) {
        pinMode(LED_BUILTIN, OUTPUT);
    }
}

//+=============================================================================
// Restart the ISR state machine
//
void IrReceiver::resume() {
    irparams.rcvstate = ReceiveState::IDLE;
}

//+=============================================================================
// hashdecode - decode an arbitrary IR code.
// Instead of decoding using a standard encoding scheme
// (e.g. Sony, NEC, RC5), the code is hashed to a 32-bit value.
//
// The algorithm: look at the sequence of MARK signals, and see if each one
// is shorter (0), the same length (1), or longer (2) than the previous.
// Do the same with the SPACE signals.  Hash the resulting sequence of 0's,
// 1's, and 2's to a 32-bit value.  This will give a unique value for each
// different code (probably), for most code systems.
//
// http://arcfn.com/2010/01/using-arbitrary-remotes-with-arduino.html
//

/*
 * Decode pulse distance protocols.
 * The mark (pulse) has constant length, the length of the space determines the bit value.
 * Each bit looks like: MARK + SPACE_1 -> 1
 *                 or : MARK + SPACE_0 -> 0
 * Data is read MSB first if not otherwise enabled.
 * Input is     results.rawbuf
 * Output is    results.value
 */
bool IrReceiver::decodePulseDistanceData(
        uint8_t numberOfBits,
        uint8_t startOffset,
        unsigned int bitMarkMicros,
        unsigned int oneSpaceMicros,
        unsigned int zeroSpaceMicros
) {
    unsigned long tDecodedData = 0;

    for (uint8_t i = 0; i < numberOfBits; i++) {
        // Check for constant length mark
        if (!matchMark(results.rawbuf[startOffset], bitMarkMicros)) {
            return false;
        }
        startOffset++;

        // Check for variable length space indicating a 0 or 1
        if (matchSpace(results.rawbuf[startOffset], oneSpaceMicros)) {
            tDecodedData = (tDecodedData << 1) | 1;
        } else if (matchSpace(results.rawbuf[startOffset], zeroSpaceMicros)) {
            tDecodedData = (tDecodedData << 1) | 0;
        } else {
            return false;
        }
        startOffset++;
    }

    results.value = tDecodedData;
    return true;
}
