#include "ir_receiver.h"

//==============================================================================
//              SSSS   AAA    MMM    SSSS  U   U  N   N   GGGG
//             S      A   A  M M M  S      U   U  NN  N  G
//              SSS   AAAAA  M M M   SSS   U   U  N N N  G  GG
//                 S  A   A  M   M      S  U   U  N  NN  G   G
//             SSSS   A   A  M   M  SSSS    UUU   N   N   GGG
//==============================================================================

const uint8_t SAMSUNG_BITS { 32 };
const uint16_t SAMSUNG_HEADER_MARK { 4500 };
const uint16_t SAMSUNG_HEADER_SPACE { 4500 };
const uint16_t SAMSUNG_BIT_MARK { 560 };
const uint16_t SAMSUNG_ONE_SPACE { 1600 };
const uint16_t SAMSUNG_ZERO_SPACE { 560 };

bool IrReceiver::decodeSamsung() {
    int offset = 1;  // Skip first space

    // Initial mark
    if (!matchMark(results.rawbuf[offset], SAMSUNG_HEADER_MARK)) {
        return false;
    }
    offset++;

    if (irparams.rawlen < (2 * SAMSUNG_BITS) + 4) {
        return false;
    }

    // Initial space
    if (!matchSpace(results.rawbuf[offset], SAMSUNG_HEADER_SPACE)) {
        return false;
    }
    offset++;

    if (!decodePulseDistanceData(SAMSUNG_BITS, offset, SAMSUNG_BIT_MARK, SAMSUNG_ONE_SPACE, SAMSUNG_ZERO_SPACE)) {
        return false;
    }

    return true;
}
