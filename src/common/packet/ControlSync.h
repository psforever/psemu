#pragma once

#include "common/bitstream.h"

class ControlSync {
public:
    unsigned short timeDiff;
    unsigned int unk;
    unsigned int field1;
    unsigned int field2;
    unsigned int field3;
    unsigned int field4;
    unsigned long long field64A;
    unsigned long long field64B;

    static ControlSync decode(BitStream& bitStream) {
        ControlSync packet;
        bitStream.read(&packet.timeDiff);
        bitStream.read(&packet.unk);
        bitStream.read(&packet.field1);
        bitStream.read(&packet.field2);
        bitStream.read(&packet.field3);
        bitStream.read(&packet.field4);
        bitStream.read(&packet.field64A);
        bitStream.read(&packet.field64B);
        return packet;
    }
};
