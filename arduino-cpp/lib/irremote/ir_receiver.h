//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Initially coded 2009 Ken Shirriff http://www.righto.com
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
//******************************************************************************
#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <stdint.h>

#ifdef ARDUINO_AVR_UNO
#include <Arduino.h>
#include <avr/pgmspace.h>
#else
#include "Arduino.h"
#endif

//------------------------------------------------------------------------------
// Information for the Interrupt Service Routine
//
const uint8_t RAW_BUFFER_LENGTH { 101 };  ///< Maximum length of raw duration buffer. Must be odd. Supports 16 + 32 bit codings.

// ISR State-Machine : Receiver States
enum class ReceiveState : uint8_t {
    IDLE,
    MARK,
    SPACE,
    STOP,
};

//------------------------------------------------------------------------------
// Pulse parms are ((X*50)-100) for the Mark and ((X*50)+100) for the Space.
// First MARK is the one after the long gap
// Pulse parameters in uSec
//

/**
 * When received, marks  tend to be too long and spaces tend to be too short.
 * To compensate for this, MARK_EXCESS_MICROS is subtracted from all marks, and added to all spaces.
 * If you set MARK_EXCESS to approx. 50us then the TSOP4838 works best.
 * At 100us it also worked, but not as well.
 * Set MARK_EXCESS to 100us and the VS1838 doesn't work at all.
 */
const uint8_t MARK_EXCESS_MICROS { 100 };

/** Relative tolerance (in percent) for some comparisons on measured data. */
const uint8_t TOLERANCE { 25 };

/** Lower tolerance for comparison of measured data */
const uint8_t LTOL { 100 - TOLERANCE };
/** Upper tolerance for comparison of measured data */
const uint8_t UTOL { 100 + TOLERANCE };

/** Minimum gap between IR transmissions, in microseconds */
const uint16_t MINIMUM_GAP { 5000 };

const uint8_t MICROS_PER_TICK { 50 };

/** Minimum gap between IR transmissions, in MICROS_PER_TICK */
const uint16_t GAP_TICKS  {MINIMUM_GAP / MICROS_PER_TICK };

//------------------------------------------------------------------------------
// IR detector output is active low
//
const uint8_t MARK { 0 }; ///< Sensor output for a mark ("flash")
const uint8_t SPACE { 1 }; ///< Sensor output for a space ("gap")

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int matchMark(uint16_t measured_ticks, unsigned int desired_us);
int matchSpace(uint16_t measured_ticks, unsigned int desired_us);

/****************************************************
 *                     RECEIVING
 ****************************************************/
/**
 * Results returned from the decoder
 */
struct decode_results {
    uint32_t value;             ///< Decoded value / command [max 32-bits]

    // next 3 values are copies of irparams values - see IRremoteint.h
    uint16_t *rawbuf;           ///< Raw intervals in 50uS ticks
};

/**
 * This struct is used for the ISR (interrupt service routine)
 * and is copied once only in state STATE_STOP, so only rcvstate needs to be volatile.
 */
struct irparams_struct {
    // The fields are ordered to reduce memory over caused by struct-padding
    volatile ReceiveState rcvstate;      ///< State Machine state
    uint8_t recvpin;                ///< Pin connected to IR data from detector
    uint8_t blinkflag;              ///< true -> enable blinking of pin on IR processing
    uint16_t rawlen;                ///< counter of entries in rawbuf
    uint16_t timer;                 ///< State timer, counts 50uS ticks.
    uint16_t rawbuf[RAW_BUFFER_LENGTH]; ///< raw data / tick counts per mark/space, first entry is the length of the gap between previous and current command
};
inline irparams_struct irparams2;

/**
 * Main class for receiving IR
 */
class IrReceiver {
public:
    static irparams_struct irparams; // NOLINT(bugprone-dynamic-static-initializers)

    /**
     * Instantiate the IrReceiver class. Multiple instantiation is not supported.
     * @param recvpin Arduino pin to use. No sanity check is made.
     */
    explicit IrReceiver(uint8_t recvpin);

    static void blink13(int blinkflag);

    /**
     * Attempt to decode the recently receive IR signal
     * @param results decode_results instance returning the decode, if any.
     * @return success of operation.
     */
    bool decode();

    /**
     * Enable IR reception.
     */
    static void enableIRIn();

    /**
     * Called to re-enable IR reception.
     */
    static void resume();

    decode_results results { 0, nullptr }; // the instance for decoding
private:
    bool decodePulseDistanceData(uint8_t numberOfBits, uint8_t startOffset, unsigned int bitMarkMicros,
                                 unsigned int oneSpaceMicros, unsigned int zeroSpaceMicros);
    bool decodeSamsung();
};

#endif // IR_RECEIVER_H
