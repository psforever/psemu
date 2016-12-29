#pragma once

#include <memory>
#include <vector>
#include "common/server.h"
#include "common/session.h"

/**
 * Top level handler for receiving network data from a server.
 */
void serverRecvHandler(Server& server, std::vector<uint8_t>& data, std::shared_ptr<Session> session);

/**
 * Pokes all sessions that need it to keep them active.
 */
void keepSessionsAlive(Server& server);
