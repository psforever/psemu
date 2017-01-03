#pragma once

#include <array>
#include <string>
#include "common/bitstream.h"
#include "common/util.h"

class ConnectToWorldRequestMessage {
public:
    std::string serverName;
    std::array<uint8_t, 32> token;
    uint32_t majorVersion;
    uint32_t minorVersion;
    uint32_t revision;
    std::string buildDate;
    uint16_t unk0;
    
    static ConnectToWorldRequestMessage decode(BitStream& bitStream) {
        ConnectToWorldRequestMessage packet;
        bitStream.read(packet.serverName);
        bitStream.read(packet.token);
        bitStream.read(packet.majorVersion);
        bitStream.read(packet.minorVersion);
        bitStream.read(packet.revision);
        bitStream.read(packet.buildDate);
        bitStream.read(packet.unk0);
        return packet;
    }
};
