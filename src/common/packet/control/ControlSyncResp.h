#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class ControlSyncResp {
public:
    uint16_t timeDiff;
    uint32_t serverTick;
    uint64_t field1;
    uint64_t field2;
    uint64_t field3;
    uint64_t field4;

    void encode(BitStream& bitStream) {
        uint8_t controlByte = 0x00;
        bitStream.write(controlByte);
        uint8_t opcode = OP_ControlSyncResp;
        bitStream.write(opcode);

        bitStream.write(timeDiff);
        bitStream.write(serverTick);
        bitStream.write(field1);
        bitStream.write(field2);
        bitStream.write(field3);
        bitStream.write(field4);
    }
};
