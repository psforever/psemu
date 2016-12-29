#pragma once

#include <map>
#include <memory>
#include <vector>
#include "asio.hpp"
#include "session.h"

using asio::ip::udp;

/**
 * Represents a server listening on a particular port.
 * Automaticaly starts listening upon construction.
 */
class Server {
public:
    Server(short port, void(*recvHandler)(Server&, std::vector<unsigned char>&, std::shared_ptr<Session> session)) :
        serverSocket(ioService, udp::endpoint(udp::v4(), port)),
        recvHandler(recvHandler) {
        receive();
    }

    /**
     * Checks for any received data and passes the data to the receive handler.
     */
    void poll();

    /**
     * Sends data to a session's endpoint.
     */
    void send(std::vector<unsigned char>& data, std::shared_ptr<Session> session);

    /**
     * @return The port the server is listening on.
     */
    unsigned short getPort() const;

    /**
     * @return A map of all sessions that the server knows about.
     */
    const std::map<asio::ip::address, std::shared_ptr<Session>>& getSessionMap() const;

private:
    /**
     * @return An existing session with the endpoint's address, or a new session if there isn't one.
     */
    std::shared_ptr<Session> getOrMakeSession(udp::endpoint endpoint);

    /**
     * Creates a new async receive request.
     */
    void receive();

    asio::io_service ioService;
    udp::socket serverSocket;
    udp::endpoint clientEndpoint;
    std::array<unsigned char, 2048> recvBuf;
    std::map<asio::ip::address, std::shared_ptr<Session>> sessions;
    void(*recvHandler)(Server&, std::vector<unsigned char>&, std::shared_ptr<Session> session);
};
