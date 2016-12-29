#pragma once

#include "common/bitstream.h"
#include <vector>

class PacketHeader {
public:
    unsigned char packetType;
    bool unused;
    bool secured;
    bool advanced;
    bool lenSpecified;
    unsigned short seqNum;

    static PacketHeader decode(BitStream& bitStream) {
        PacketHeader header;
        unsigned char flags;
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
        unsigned char flags = 0;
        flags |= (packetType & 0b1111) << 4;
        flags |= ((unsigned char)unused) << 3;
        flags |= ((unsigned char)secured) << 2;
        flags |= ((unsigned char)advanced) << 1;
        flags |= ((unsigned char)lenSpecified);

        bitStream.write(&flags);
        bitStream.write(&seqNum);
    }
};
