#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class SlottedMetaAck {
public:
    uint8_t slot;
    uint16_t subslot;

    void encode(BitStream& bitStream) {
        uint8_t opcode = 0x00;
        bitStream.write(opcode);
        uint8_t controlOpcode = OP_RelatedB0 + slot % 4;
        bitStream.write(controlOpcode);

        bitStream.write(subslot);
    }
};
