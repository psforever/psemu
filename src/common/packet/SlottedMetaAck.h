#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class SlottedMetaAck {
public:
    unsigned char slot;
    unsigned short subslot;

    void encode(BitStream& bitStream) {
        unsigned char opcode = 0x00;
        bitStream.write(&opcode);
        unsigned char controlOpcode = OP_RelatedB0 + slot % 4;
        bitStream.write(&controlOpcode);

        bitStream.write(&subslot);
    }
};
