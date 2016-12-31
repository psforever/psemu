#pragma once

#include <array>
#include "common/bitstream.h"

class ClientChallengeXchg {
public:
    uint8_t unk0;
    uint8_t unk1;
    uint32_t clientTime;
    std::array<uint8_t, 12> challenge;
    uint8_t unkEndChallenge;
    uint8_t unkObjects0;
    uint16_t unkObjectType;
    uint32_t unk2;
    uint16_t pLen;
    std::array<uint8_t, 16> p;
    uint16_t gLen;
    std::array<uint8_t, 16> g;
    uint8_t unkEnd0;
    uint8_t unkEnd1;
    uint8_t unkObjects1;
    uint32_t unk3;
    uint8_t unkEnd2;

    static ClientChallengeXchg decode(BitStream& bitStream) {
        ClientChallengeXchg packet;
        bitStream.read(&packet.unk0);
        bitStream.read(&packet.unk1);
        bitStream.read(&packet.clientTime);
        bitStream.read(&packet.challenge);
        bitStream.read(&packet.unkEndChallenge);
        bitStream.read(&packet.unkObjects0);
        bitStream.read(&packet.unkObjectType);
        bitStream.read(&packet.unk2);
        bitStream.read(&packet.pLen);
        bitStream.read(&packet.p);
        bitStream.read(&packet.gLen);
        bitStream.read(&packet.g);
        bitStream.read(&packet.unkEnd0);
        bitStream.read(&packet.unkEnd1);
        bitStream.read(&packet.unkObjects1);
        bitStream.read(&packet.unk3);
        bitStream.read(&packet.unkEnd2);
        return packet;
    }
};
