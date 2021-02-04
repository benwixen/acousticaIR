#include "unity.h"
#include "infra.h"
#include "arduino_mock.h"
#include "stdio.h"

const uint32_t sensor_data[] {
        2631068, 4524, 4484,
        572, 1668,
        572, 1668,
        572, 1664,
        572, 576,
        552, 568,
        544, 572,
        556, 564,
        548, 572,
        552, 1660,
        580, 1660,
        580, 1660,
        576, 568,
        548, 572,
        552, 568,
        548, 572,
        552, 568,
        544, 1668,
        572, 1668,
        572, 572,
        552, 1660,
        580, 568,
        544, 576,
        552, 568,
        544, 576,
        548, 568,
        548, 572,
        552, 1660,
        580, 568,
        544, 1668,
        572, 1668,
        572, 1668,
        572, 1664,
};

namespace infra {
    void listenToIrReceivePin();
}

void shouldDecodeIrSignal() {
    uint32_t timePassed { 0 };
    for (const auto sensor_diff : sensor_data) {
        timePassed += sensor_diff;
        NEXT_MICROS = timePassed;
        infra::listenToIrReceivePin();
    }
    uint32_t decodedValue = infra::checkForValue();
    TEST_ASSERT_EQUAL(3'772'829'743, decodedValue);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(shouldDecodeIrSignal);
    UNITY_END();
    return 0;
}
//#include "sim_avr.h"
//#include "sim_elf.h"
//#include "stdio.h"
//
////#include "avr_mcu_section.h"
////AVR_MCU(F_CPU, "atmega88");
//
//avr_t * avr = NULL;
//
//void test_function_calculator_addition() {
//    elf_firmware_t f;
//    const char * fname = ".pio/build/uno/firmware.elf";
//    printf("Firmware pathname is %s\n", fname);
//    elf_read_firmware(fname, &f);
//    printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);
//
//    avr = avr_make_mcu_by_name("atmega328p");
//    if (!avr) {
//        fprintf(stderr, "%s: AVR '%s' not known\n", "jeje", f.mmcu);
//        return;
//    }
//    avr_init(avr);
////    avr_load_firmware(avr, &f);
//
//    printf( "\nDemo launching:\n");
//    int state = cpu_Running;
//    while ((state != cpu_Done) && (state != cpu_Crashed)) {
//        printf("loopzy");
//        state = avr_run(avr);
//        avr->
//    }
//
//    TEST_ASSERT_EQUAL(32, 32);
//}
//
