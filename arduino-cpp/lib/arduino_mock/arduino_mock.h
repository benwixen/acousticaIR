#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-dynamic-static-initializers"

#include <stdint.h>

const uint8_t LED_BUILTIN { 9 };
#define F_CPU 1000000
#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define HIGH 1
#define CHANGE 3

#define ISR(vec) void isr()
#define _BV(reg) reg
#define F(str) str

extern volatile uint8_t TCCR2A;
extern volatile uint8_t TCCR2B;
extern volatile uint8_t OCR2A;
extern volatile uint8_t OCIE2A;
extern volatile uint8_t TCNT2;
extern volatile uint8_t WGM21;
extern volatile uint8_t CS21;
extern volatile uint8_t TIMSK2;
extern volatile uint8_t PORTB;

extern volatile uint8_t NEXT_READ;
extern volatile uint32_t NEXT_MICROS;

void delay(uint16_t ms);
void noInterrupts();
void interrupts();
void pinMode(uint8_t pin, uint8_t mode);
uint8_t digitalPinToInterrupt(uint8_t pin);
void attachInterrupt(uint8_t interrupt, void (*pin)(), int i);
uint8_t digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t value);
uint32_t micros();

class SerialMock {
public:
    void begin(int baud_rate) const {}
    void print(const char* str) const {}
    void print(const uint8_t num) const {}
    void println(const char* str) const {}
    void println(const uint8_t num) const {}
    void println() const {}
};

const SerialMock Serial;

#pragma clang diagnostic pop
#endif // ARDUINO_MOCK_H

