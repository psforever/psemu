#pragma once

#include "common/bitstream.h"

class ClientStart {
public:
    uint32_t unk0;
    uint32_t clientNonce;
    uint32_t unk1;

    static ClientStart decode(BitStream& bitStream) {
        ClientStart packet;
        bitStream.read(packet.unk0);
        bitStream.read(packet.clientNonce);
        bitStream.read(packet.unk1);
        return packet;
    }
};
