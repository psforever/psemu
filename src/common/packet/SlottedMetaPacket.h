#pragma once

#include <vector>
#include "common/bitstream.h"

class SlottedMetaPacket {
public:
    unsigned char slot;
    unsigned short subslot;
    std::vector<unsigned char> rest;

    static SlottedMetaPacket decode(BitStream& bitStream, unsigned char slot) {
        SlottedMetaPacket packet;
        packet.slot = slot;
        bitStream.read(&packet.subslot);

        size_t remainingBytes = bitStream.getRemainingBytes();
        packet.rest.resize(remainingBytes);
        bitStream.readBytes(packet.rest.data(), remainingBytes);
        return packet;
    }
};
