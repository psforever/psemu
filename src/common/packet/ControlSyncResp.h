#pragma once

#include "opcodes.h"
#include "common/bitstream.h"

class ControlSyncResp {
public:
    unsigned short timeDiff;
    unsigned int serverTick;
    unsigned long long field1;
    unsigned long long field2;
    unsigned long long field3;
    unsigned long long field4;

    void encode(BitStream& bitStream) {
        unsigned char controlByte = 0x00;
        bitStream.write(&controlByte);
        unsigned char opcode = OP_ControlSyncResp;
        bitStream.write(&opcode);

        bitStream.write(&timeDiff);
        bitStream.write(&serverTick);
        bitStream.write(&field1);
        bitStream.write(&field2);
        bitStream.write(&field3);
        bitStream.write(&field4);
    }
};
