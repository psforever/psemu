#pragma once

#include "common/bitstream.h"

class CharacterRequestMessage {
public:
    enum CharacterRequestAction {
        CRA_Select,
        CRA_Delete
    };

    unsigned int charId;
    unsigned int action;

    static CharacterRequestMessage decode(BitStream& bitStream) {
        CharacterRequestMessage packet;
        bitStream.read(&packet.charId);
        bitStream.read(&packet.action);
        return packet;
    }
};
