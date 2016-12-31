#pragma once

#include "common/bitstream.h"

class CharacterRequestMessage {
public:
    enum CharacterRequestAction {
        CRA_Select,
        CRA_Delete
    };

    uint32_t charId;
    uint32_t action;

    static CharacterRequestMessage decode(BitStream& bitStream) {
        CharacterRequestMessage packet;
        bitStream.read(&packet.charId);
        bitStream.read(&packet.action);
        return packet;
    }
};
