#pragma once

#include <vector>
#include "common/bitstream.h"

class PacketHeader {
public:
    uint8_t packetType;
    bool unused;
    bool secured;
    bool advanced;
    bool lenSpecified;
    uint16_t seqNum;

    static PacketHeader decode(BitStream& bitStream) {
        PacketHeader header;
        uint8_t flags;
        bitStream.read(&flags);
        header.packetType = (flags & 0b11110000) >> 4;
        header.unused = (flags & 0b1000) >> 3;
        header.secured = (flags & 0b100) >> 2;
        header.advanced = (flags & 0b10) >> 1;
        header.lenSpecified = (flags & 0b1);
        bitStream.read(&header.seqNum);
        return header;
    }

    void encode(BitStream& bitStream) {
        uint8_t flags = 0;
        flags |= (packetType & 0b1111) << 4;
        flags |= ((uint8_t)unused) << 3;
        flags |= ((uint8_t)secured) << 2;
        flags |= ((uint8_t)advanced) << 1;
        flags |= ((uint8_t)lenSpecified);

        bitStream.write(&flags);
        bitStream.write(&seqNum);
    }
};
