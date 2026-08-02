#ifndef THORSANVIL_NISSE_MCP_JSONRPC_SERVER_H
#define THORSANVIL_NISSE_MCP_JSONRPC_SERVER_H

#include "NisseMCPConfig.h"

#include "Server.h"
#include "JsonRPCSession.h"
#include "JsonRPCCore.h"

namespace ThorsAnvil::Nisse::MCP
{

class JsonRPCServer: public Server
{
    // JSONRPCServer has a single session.
    JsonRPCSession      session;
    JsonRPCCore         core;

    public:
        using Server::Server;

        virtual JsonRPCCore& getCore() override;
    private:
        virtual void handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) override;
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "JsonRPCServer.source"
#endif

#endif
