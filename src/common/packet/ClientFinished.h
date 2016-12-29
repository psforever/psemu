#pragma once

#include <array>
#include "common/bitstream.h"

class ClientFinished {
public:
    unsigned char unkObjectType;
    unsigned short pubKeyLen;
    std::array<unsigned char, 16> pubKey;
    unsigned short unk0;
    std::array<unsigned char, 12> challengeResult;

    static ClientFinished decode(BitStream& bitStream) {
        ClientFinished packet;
        bitStream.read(&packet.unkObjectType);
        bitStream.read(&packet.pubKeyLen);
        bitStream.read(&packet.pubKey);
        bitStream.read(&packet.unk0);
        bitStream.read(&packet.challengeResult);
        return packet;
    }
};
