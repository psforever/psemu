#include <iostream>
#include "asio.hpp"
#include "common/log.h"

using asio::ip::udp;

int main(int argc, char* argv[]) {
    try {
        /*if (argc != 3) {
            std::cerr << "Usage: client <host> <port>\n";
            return 1;
        }*/

        const char* host = "192.168.1.171";//argv[1]
        const char* port = "51000";//argv[2]

        asio::io_service ioService;

        udp::socket socket(ioService, udp::endpoint(udp::v4(), 0));

        udp::resolver resolver(ioService);
        udp::endpoint serverEndpoint = *resolver.resolve({ udp::v4(), host, port });

        while (true) {
            std::cout << "Enter message: ";
            char request[2048];
            std::cin.getline(request, sizeof(request));
            size_t requestLength = std::strlen(request);

            unsigned char reply[2048];
            udp::endpoint senderEndpoint;
            size_t replyLength;

            unsigned char hardcodedStuff[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x26, 0x1E, 0x27, 0x00, 0x00, 0x01, 0xF0 };
            socket.send_to(asio::buffer(hardcodedStuff, sizeof(hardcodedStuff)), serverEndpoint);
            replyLength = socket.receive_from(asio::buffer(reply, sizeof(reply)), senderEndpoint);
            std::cout << "Reply is:" << strHex(reply, replyLength) << std::endl;

            unsigned char hardcodedStuff2[] = { 0x32, 0x00, 0x00, 0x01, 0x01, 0x81, 0x5a, 0x5f, 0x58, 0x83, 0x83, 0x20, 0xa4, 0x66, 0x0a, 0x1f, 0x95, 0xaf, 0x74, 0x10, 0xf5, 0x00, 0x01, 0x00, 0x02, 0xff, 0x24, 0x00, 0x00, 0x10, 0x00, 0xcb, 0x62, 0xd4, 0x44, 0xd2, 0xbd, 0x58, 0xa6, 0xef, 0x4e, 0x6a, 0xfa, 0xe2, 0x2c, 0x44, 0xdf, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x03, 0x07, 0x00, 0x00, 0x00 };
            socket.send_to(asio::buffer(hardcodedStuff2, sizeof(hardcodedStuff2)), serverEndpoint);
            replyLength = socket.receive_from(asio::buffer(reply, sizeof(reply)), senderEndpoint);
            std::cout << "Reply is:" << strHex(reply, replyLength) << std::endl;

            unsigned char hardcodedStuff3[] = { 0x32, 0x01, 0x00, 0x10, 0x10, 0x00, 0x9e, 0x55, 0xaa, 0xd8, 0xdb, 0x2a, 0x9d, 0x50, 0xd1, 0xe1, 0x1c, 0x00, 0xde, 0x1f, 0xd5, 0x3e, 0x01, 0x14, 0x74, 0xad, 0x42, 0x61, 0xf7, 0xf9, 0xbe, 0xec, 0x65, 0xba, 0xe8, 0xc1 };
            socket.send_to(asio::buffer(hardcodedStuff3, sizeof(hardcodedStuff3)), serverEndpoint);
            replyLength = socket.receive_from(asio::buffer(reply, sizeof(reply)), senderEndpoint);
            std::cout << "Reply is:" << strHex(reply, replyLength) << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
