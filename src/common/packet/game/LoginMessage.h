#pragma once

#include <array>
#include <string>
#include "common/bitstream.h"
#include "common/util.h"

class LoginMessage {
public:
    enum CredentialsType {
        CT_UserPassword,
        CT_UserToken
    };

    uint32_t majorVersion;
    uint32_t minorVersion;
    std::string buildDate;
    bool credentialsType;
    std::string username;
    std::string password;
    std::array<uint8_t, 32> token;
    uint32_t revision;

    static LoginMessage decode(BitStream& bitStream) {
        LoginMessage packet;
        bitStream.read(packet.majorVersion);
        bitStream.read(packet.minorVersion);
        bitStream.read(packet.buildDate);
        packet.credentialsType = bitStream.readBit();
        if (packet.credentialsType == CT_UserPassword) {
            bitStream.read(packet.username);
            bitStream.read(packet.password);
        } else {
            // TODO: Fix this. Not sure if it's supposed to be padded or aligned or what.
            bitStream.read(packet.token);
            bitStream.read(packet.username);
        }
        bitStream.read(packet.revision);
        return packet;
    }
};
