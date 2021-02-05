#include "usart.h"

#include <avr/io.h>
#define BAUD 9600
#include <util/setbaud.h>

namespace {
    inline void setBaudRate() {
        UBRR0H = UBRRH_VALUE;
        UBRR0L = UBRRL_VALUE;
    }

    inline void enableUsartReceive() {
        UCSR0B = (1 << RXEN0);
    }

    inline void enableUsartTransmit() {
        UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    }

    inline void set8bitCharacterSize() {
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    }

    inline void transmitByte(uint8_t data) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = data;
    }
}

namespace usart {
    void initUsart() {
        setBaudRate();
        enableUsartTransmit();
        set8bitCharacterSize();
    }

    void printString(const char* str) {
        uint8_t i = 0;
        while (str[i]) {
            transmitByte(str[i]);
            i++;
        }
    }
}
