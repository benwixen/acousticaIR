//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Initially coded 2009 Ken Shirriff http://www.righto.com
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//******************************************************************************

#include "ir_receiver.h"

irparams_struct IrReceiver::irparams {};

inline uint16_t calculateTicksLow(const unsigned int desired_us) {
    return ((long)desired_us * LTOL) / (MICROS_PER_TICK * 100);
}

inline uint16_t calculateTicksHigh(const unsigned int desired_us) {
    return (((long)desired_us * UTOL) / (MICROS_PER_TICK * 100)) + 1;
}

//+========================================================
// Due to sensor lag, when received, Marks tend to be 100us too long
//
int matchMark(uint16_t measured_ticks, unsigned int desired_us) {
    // compensate for marks exceeded by demodulator hardware
    uint16_t ticks_low = calculateTicksLow(desired_us + MARK_EXCESS_MICROS);
    uint16_t ticks_high = calculateTicksHigh(desired_us + MARK_EXCESS_MICROS);
    bool passed = ((measured_ticks >= ticks_low)
            && (measured_ticks <= ticks_high));
    return passed;
}

//+========================================================
// Due to sensor lag, when received, Spaces tend to be 100us too short
//
int matchSpace(uint16_t measured_ticks, unsigned int desired_us) {
    // compensate for marks exceeded and spaces shortened by demodulator hardware
    uint16_t ticks_low = calculateTicksLow(desired_us - MARK_EXCESS_MICROS);
    uint16_t ticks_high = calculateTicksHigh(desired_us - MARK_EXCESS_MICROS);
    bool passed = ((measured_ticks >= ticks_low)
            && (measured_ticks <= ticks_high));
    return passed;
}

//+=============================================================================
// Interrupt Service Routine - Fires every 50uS
// TIMER2 interrupt code to collect raw data.
// Widths of alternating SPACE, MARK are recorded in rawbuf.
// Recorded in ticks of 50uS [microseconds, 0.000050 seconds]
// 'rawlen' counts the number of entries recorded so far.
// First entry is the SPACE between transmissions.
// As soon as a the first [SPACE] entry gets long:
//   Ready is set; State switches to IDLE; Timing of SPACE continues.
// As soon as first MARK arrives:
//   Gap width is recorded; Ready is cleared; New logging starts
//
//extern "C" void TIMER2_COMPA_vect(void) __attribute__((signal, used, externally_visible));
//void TIMER2_COMPA_vect(void) {
ISR (TIMER2_COMPA_vect) {

    // Read if IR Receiver -> SPACE [xmt LED off] or a MARK [xmt LED on]
    // digitalRead() is very slow. Optimisation is possible, but makes the code unportable
    auto irdata = (uint8_t) digitalRead(IrReceiver::irparams.recvpin);

    IrReceiver::irparams.timer++;  // One more 50uS tick
    if (IrReceiver::irparams.rawlen >= RAW_BUFFER_LENGTH) {
        // Flag up a read overflow; Stop the State Machine
        IrReceiver::irparams.rcvstate = ReceiveState::STOP;
    }

    /*
     * Due to a ESP32 compiler bug https://github.com/espressif/esp-idf/issues/1552 no switch statements are possible for ESP32
     * So we change the code to if / else if
     */
//    switch (IrReceiver::irparams.rcvstate) {
    //......................................................................
    if (IrReceiver::irparams.rcvstate == ReceiveState::IDLE) { // In the middle of a gap
        if (irdata == MARK) {
            if (IrReceiver::irparams.timer < GAP_TICKS) {  // Not big enough to be a gap.
                IrReceiver::irparams.timer = 0;
            } else {
                // Gap just ended; Record gap duration; Start recording transmission
                // Initialize all state machine variables
                IrReceiver::irparams.rawlen = 0;
                IrReceiver::irparams.rawbuf[IrReceiver::irparams.rawlen++] = IrReceiver::irparams.timer;
                IrReceiver::irparams.timer = 0;
                IrReceiver::irparams.rcvstate = ReceiveState::MARK;
            }
        }
    } else if (IrReceiver::irparams.rcvstate == ReceiveState::MARK) {  // Timing Mark
        if (irdata == SPACE) {   // Mark ended; Record time
            IrReceiver::irparams.rawbuf[IrReceiver::irparams.rawlen++] = IrReceiver::irparams.timer;
            IrReceiver::irparams.timer = 0;
            IrReceiver::irparams.rcvstate = ReceiveState::SPACE;
        }
    } else if (IrReceiver::irparams.rcvstate == ReceiveState::SPACE) {  // Timing Space
        if (irdata == MARK) {  // Space just ended; Record time
            IrReceiver::irparams.rawbuf[IrReceiver::irparams.rawlen++] = IrReceiver::irparams.timer;
            IrReceiver::irparams.timer = 0;
            IrReceiver::irparams.rcvstate = ReceiveState::MARK;

        } else if (IrReceiver::irparams.timer > GAP_TICKS) {  // Space
            // A long Space, indicates gap between codes
            // Flag the current code as ready for processing
            // Switch to STOP
            // Don't reset timer; keep counting Space width
            IrReceiver::irparams.rcvstate = ReceiveState::STOP;
        }
    } else if (IrReceiver::irparams.rcvstate == ReceiveState::STOP) {  // Waiting; Measuring Gap
        if (irdata == MARK) {
            IrReceiver::irparams.timer = 0;  // Reset gap timer
        }
    }

    // If requested, flash LED while receiving IR data
    if (IrReceiver::irparams.blinkflag) {
        if (irdata == MARK) {
            (PORTB |= 0b00100000);   // if no user defined LED pin, turn default LED pin for the hardware on
        } else {
            (PORTB &= 0b11011111);   // if no user defined LED pin, turn default LED pin for the hardware on
        }
    }
}
