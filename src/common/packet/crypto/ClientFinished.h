#pragma once

#include <array>
#include "common/bitstream.h"

class ClientFinished {
public:
    uint8_t unkObjectType;
    uint16_t pubKeyLen;
    std::array<uint8_t, 16> pubKey;
    uint16_t unk0;
    std::array<uint8_t, 12> challengeResult;

    static ClientFinished decode(BitStream& bitStream) {
        ClientFinished packet;
        bitStream.read(packet.unkObjectType);
        bitStream.read(packet.pubKeyLen);
        bitStream.read(packet.pubKey);
        bitStream.read(packet.unk0);
        bitStream.read(packet.challengeResult);
        return packet;
    }
};
