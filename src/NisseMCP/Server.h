#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "NisseMCPConfig.h"

#include "NisseHTTP/Server.h"

#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

class JsonRPCCore;

class Server: public ThorsAnvil::Nisse::HTTP::Server
{
    public:
        Server(std::string_view slot, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079});
        virtual ~Server();
        virtual JsonRPCCore& getCore() = 0;

    private:
        virtual void handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) = 0;
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "Server.source"
#endif

#endif
