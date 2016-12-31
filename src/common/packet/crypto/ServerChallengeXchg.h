#pragma once

#include <array>
#include "common/bitstream.h"

class ServerChallengeXchg {
public:
    uint8_t unk0;
    uint8_t unk1;
    uint32_t serverTime;
    std::array<uint8_t, 12> challenge;
    uint8_t unkChallengeEnd;
    uint8_t unkObjects;
    std::array<uint8_t, 7> unk2;
    uint16_t pubKeyLen;
    std::array<uint8_t, 16> pubKey;
    uint8_t unk3;

    void encode(BitStream& bitStream) {
        bitStream.write(&unk0);
        bitStream.write(&unk1);
        bitStream.write(&serverTime);
        bitStream.write(&challenge);
        bitStream.write(&unkChallengeEnd);
        bitStream.write(&unkObjects);
        bitStream.write(&unk2);
        bitStream.write(&pubKeyLen);
        bitStream.write(&pubKey);
        bitStream.write(&unk3);
    }
};
