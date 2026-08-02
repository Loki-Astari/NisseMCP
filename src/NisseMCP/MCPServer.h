#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseMCPConfig.h"
#include "MCPSession.h"
#include "MCPJanitor.h"
#include "MCPCore.h"
#include "Server.h"

#include "NisseServer/TimerAction.h"

#include <boost/uuid.hpp>

#include <chrono>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

struct MCPServerConfig
{
    std::string_view        serverName;
    std::string_view        allowedOrigin;                              // Origin header validated against this string.
    std::string_view        slot            = "/mpc";                   // HTTP endpoint.
    ProtocolRange           protocolInfo    = {Protocol::v2025_11_25, Protocol::v2025_11_25};
    Duration                janitorCheckTime= std::chrono::seconds{30}; // When the janitor thread runs.
    Duration                sessionTimeout  = std::chrono::minutes{30}; // Session time out after 30 minutes of no activity
    Duration                initHandShake   = std::chrono::seconds{15}; // Init handshake needs to be completed in 10 seconds.
                                                                        // Note: Shorter than janitor thread so it wil pick
                                                                        // up timed out handskes quickly but it may take upto 45 seconds.
};

class MCPServer: public Server
{
    SessionMap          sessionMap;
    ProtocolRange       protocolInfo;
    MCPCore             core;
    MCPJanitor          janitor;
    std::string         serverName;
    std::string         allowedOrigin;

    public:
        MCPServer(MCPServerConfig const& config, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079});

        virtual JsonRPCCore& getCore() override;
    private:
                void                removeSession(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response);
        virtual void                handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) override;
                MCPSession&         validateRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, std::string_view originAllowed);
                std::string_view    getMethodName(ThorsAnvil::Nisse::HTTP::Request const& request) const;
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPServer.source"
#endif

#endif
