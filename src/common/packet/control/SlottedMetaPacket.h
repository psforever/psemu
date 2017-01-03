#pragma once

#include <vector>
#include "common/bitstream.h"

class SlottedMetaPacket {
public:
    uint8_t slot;
    uint16_t subslot;
    std::vector<uint8_t> rest;

    static SlottedMetaPacket decode(BitStream& bitStream, uint8_t slot) {
        SlottedMetaPacket packet;
        packet.slot = slot;
        bitStream.read(packet.subslot);

        size_t remainingBytes = bitStream.getRemainingBytes();
        packet.rest.resize(remainingBytes);
        bitStream.readBytes(packet.rest.data(), remainingBytes);
        return packet;
    }
};
