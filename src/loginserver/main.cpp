#include "server.h"
#include "common/util.h"
#include "common/packet/pkt_test.h"
#include "common/bitstream_test.h"

int main(int argc, char* argv[]) {
    /*if (argc != 2) {
        std::cerr << "Usage: loginserver <port>\n";
        return 1;
    }*/

    testPacketCodingControl();
    testPacketCodingCrypto();
    testPacketCodingGame();
    testBitstream();

    // Just creating both servers in one process for now...
    const char* port = "51000";//argv[1]
    Server loginServer(std::atoi(port), serverRecvHandler);

    port = "51001";
    Server worldServer(std::atoi(port), serverRecvHandler);

    while (true) {
        loginServer.poll();
        utilSleep(50);

        worldServer.poll();
        utilSleep(50);

        keepSessionsAlive(worldServer);
    }

    return 0;
}


