#pragma once

#include <array>
#include "common/bitstream.h"

class ClientChallengeXchg {
public:
    unsigned char unk0;
    unsigned char unk1;
    unsigned int clientTime;
    std::array<unsigned char, 12> challenge;
    unsigned char unkEndChallenge;
    unsigned char unkObjects0;
    unsigned short unkObjectType;
    unsigned int unk2;
    unsigned short pLen;
    std::array<unsigned char, 16> p;
    unsigned short gLen;
    std::array<unsigned char, 16> g;
    unsigned char unkEnd0;
    unsigned char unkEnd1;
    unsigned char unkObjects1;
    unsigned int unk3;
    unsigned char unkEnd2;

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
