#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseMCPConfig.h"
#include "Context.h"

#include "Server.h"
#include "MCPCore.h"

#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{
    struct MCPServerConfig
    {
        std::string_view        allowedOrigin;
        std::string_view        slot;
        Protocol                protocol;
    };
    class MCPServer: public Server
    {
        MCPCore             core;
        std::string         allowedOrigin;

        public:
            MCPServer(MCPServerConfig const& config, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079});

            virtual JsonRPCCore& getCore() override;
        private:
            virtual void handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) override;
            bool validateRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, std::string_view originAllowed);
    };
}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPServer.source"
#endif

#endif
