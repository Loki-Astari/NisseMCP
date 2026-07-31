#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseMCPConfig.h"
#include "Session.h"
#include "MCPCore.h"
#include "Server.h"

#include "NisseServer/TimerAction.h"

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
        Duration                sessionTimeout  = std::chrono::minutes{30}; // Session time out after 30 minutes of no activity
        Duration                initHandShake   = std::chrono::seconds{10}; // Init handshake needs to be completed in 10 seconds.
    };

    using SessionMap = std::map<boost::uuids::uuid, MCPSession>;

    class MCPServerContext: public ServerContext
    {
        public:
            MCPServerContext(MCPSession& session, ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
                : ServerContext(session, request, response)
            {}

            MCPSession& getMCPSession() {return dynamic_cast<MCPSession&>(session);}
    };

    class MCPJanitor: public ThorsAnvil::Nisse::Server::TimerAction
    {
        SessionMap&     sessionMap;
        Duration        sessionTimeout;
        Duration        initHandShake;
        public:
            MCPJanitor(SessionMap& sessionMap, Duration sessionTimeout, Duration initHandShake)
                : sessionMap{sessionMap}
                , sessionTimeout{sessionTimeout}
                , initHandShake{initHandShake}
            {}
            virtual void handleRequest(int /*timerId*/) override
            {
                for (auto loop = std::begin(sessionMap); loop != std::end(sessionMap); ++loop) {
                    if (sessionTimeout == Duration{1'0000'000}) {
                        initHandShake = Duration{1};
                    }
                }
            }
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
