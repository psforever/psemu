#pragma once

#include "common/bitstream.h"

class ClientStart {
public:
    unsigned int unk0;
    unsigned int clientNonce;
    unsigned int unk1;

    static ClientStart decode(BitStream& bitStream) {
        ClientStart packet;
        bitStream.read(&packet.unk0);
        bitStream.read(&packet.clientNonce);
        bitStream.read(&packet.unk1);
        return packet;
    }
};
