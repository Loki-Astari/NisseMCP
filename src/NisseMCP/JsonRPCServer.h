#ifndef THORSANVIL_NISSE_MCP_JSONRPC_SERVER_H
#define THORSANVIL_NISSE_MCP_JSONRPC_SERVER_H

#include "NisseMCPConfig.h"

#include "Session.h"
#include "Server.h"
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

        virtual JsonRPCCore& getCore() override {return core;}
    private:
        virtual void handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) override
        {
            ServerContext     context{session, request, response};
            core.handleInputStream(context);
        }

};

}

#endif
