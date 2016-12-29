#pragma once

#include <array>
#include <string>
#include <vector>
#include "common/bitstream.h"

class ObjectCreateMessage {
public:
    unsigned int streamLength;
    bool hasParent;
    unsigned short parentGuid;
    unsigned short objectClass;
    unsigned short guid;
    unsigned int parentSlotIndex;
    std::vector<unsigned char> rest;

    static ObjectCreateMessage decode(BitStream& bitStream) {
        ObjectCreateMessage packet;
        bitStream.read(&packet.streamLength);
        packet.hasParent = !bitStream.readBit();
        if (packet.hasParent) {
            bitStream.read(&packet.parentGuid);
        }
        bitStream.readBits((unsigned char*)&packet.objectClass, 11);
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
