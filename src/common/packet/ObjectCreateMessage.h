#pragma once

#include <array>
#include <string>
#include <vector>
#include "common/bitstream.h"

class ObjectCreateMessage {
public:
    uint32_t streamLength;
    bool hasParent;
    uint16_t parentGuid;
    uint16_t objectClass;
    uint16_t guid;
    uint32_t parentSlotIndex;
    std::vector<uint8_t> rest;

    static ObjectCreateMessage decode(BitStream& bitStream) {
        ObjectCreateMessage packet;
        bitStream.read(&packet.streamLength);
        packet.hasParent = !bitStream.readBit();
        if (packet.hasParent) {
            bitStream.read(&packet.parentGuid);
        }
        bitStream.readBits((uint8_t*)&packet.objectClass, 11);
        bitStream.read(&packet.guid);
        if (packet.hasParent) {
            // TODO: NEED TO READ INTO THE parentSlotIndex!!! it uses same decoding as the size of a string (so just reuse that)
        }
        // TODO: Not good to read up the entirety of the rest of the bitstream! Probably use streamLength?
        size_t remainingBytes = bitStream.getRemainingBytes();
        packet.rest.resize(remainingBytes);
        bitStream.readBytes(packet.rest.data(), remainingBytes);
        return packet;
    }
};
