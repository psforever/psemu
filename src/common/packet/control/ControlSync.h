#pragma once

#include "common/bitstream.h"

class ControlSync {
public:
    uint16_t timeDiff;
    uint32_t unk;
    uint32_t field1;
    uint32_t field2;
    uint32_t field3;
    uint32_t field4;
    uint64_t field64A;
    uint64_t field64B;

    static ControlSync decode(BitStream& bitStream) {
        ControlSync packet;
        bitStream.read(packet.timeDiff);
        bitStream.read(packet.unk);
        bitStream.read(packet.field1);
        bitStream.read(packet.field2);
        bitStream.read(packet.field3);
        bitStream.read(packet.field4);
        bitStream.read(packet.field64A);
        bitStream.read(packet.field64B);
        return packet;
    }
};
