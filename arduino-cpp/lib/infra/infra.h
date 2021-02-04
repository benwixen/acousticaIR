#ifndef INFRA_H
#define INFRA_H

#ifdef ARDUINO_AVR_UNO
#include <Arduino.h>
#else
#include "arduino_mock.h"
#endif

namespace infra {
    void initInfra(uint8_t receivePin);
    uint32_t checkForValue();
    void resume();
}

#endif //INFRA_H
