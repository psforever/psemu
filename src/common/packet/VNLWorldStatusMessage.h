#pragma once

#include <array>
#include <string>
#include <vector>
#include "opcodes.h"
#include "common/bitstream.h"
#include "common/util.h"

class VNLWorldStatusMessage {
public:
    enum WorldStatus {
        WS_Up,
        WS_Down,
        WS_Locked,
        WS_Full
    };

    enum ServerType {
        ST_Development = 1,
        ST_Beta,
        ST_Released
    };

    // TODO: Fill this out
    class ConnectionInfo {
    public:

        void encode(BitStream& bitStream) const {

        }
    };

    class WorldInfo {
    public:
        std::string name;
        unsigned short status2;
        unsigned char serverType;
        unsigned char status1;
        std::vector<ConnectionInfo> connections;
        unsigned char empireNeed;

        void encode(BitStream& bitStream) const {
            writeString(bitStream, name);
            bitStream.write(&status2);
            bitStream.write(&serverType);
            bitStream.write(&status1);
            unsigned char numConnections = connections.size();
            bitStream.write(&numConnections);
            for (auto const &connection : connections) {
                connection.encode(bitStream);
            }
            bitStream.writeBits(&empireNeed, 2);
        }
    };

    std::wstring welcomeMessage;
    std::vector<WorldInfo> worlds;

    void encode(BitStream& bitStream) {
        unsigned char opcode = OP_VNLWorldStatusMessage;
        bitStream.write(&opcode);

        writeString(bitStream, welcomeMessage);
        unsigned char numWorlds = worlds.size();
        bitStream.write(&numWorlds);
        for (auto const &world : worlds) {
            world.encode(bitStream);
        }
    }
};
