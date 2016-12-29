#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include "asio.hpp"
#include "server.h"

void Server::poll() {
    ioService.poll();
}

void Server::send(std::vector<unsigned char>& data, std::shared_ptr<Session> session) {
    serverSocket.send_to(asio::buffer(data), session->clientEndpoint);
}

unsigned short Server::getPort() const {
    return serverSocket.local_endpoint().port();
}

const std::map<asio::ip::address, std::shared_ptr<Session>>& Server::getSessionMap() const {
    return sessions;
}

std::shared_ptr<Session> Server::getOrMakeSession(udp::endpoint endpoint) {
    // TODO: Only make the session if the incoming packet is an OP_ClientStart control packet, else drop and ignore
    asio::ip::address addr = endpoint.address();

    auto session = sessions.find(addr);
    if (session == sessions.end()) {
        session = sessions.emplace(addr, std::make_shared<Session>(endpoint)).first;
    }

    return (*session).second;
}

void Server::receive() {
    serverSocket.async_receive_from(asio::buffer(recvBuf), clientEndpoint,
        [this](std::error_code errorCode, std::size_t bytesReceived) {
        if (!errorCode && bytesReceived > 0) {
            // TODO: Don't -really- need to copy the buffer here.
            std::vector<unsigned char> dataBuf(recvBuf.begin(), recvBuf.begin() + bytesReceived);
            recvHandler(*this, dataBuf, getOrMakeSession(clientEndpoint));
        } else {
            std::cout << "Net error: \"" << errorCode.message() << "\", recvd " << bytesReceived << " bytes" << std::endl;
        }

        // Call receive again to wait for more data.
        receive();
    });
}
