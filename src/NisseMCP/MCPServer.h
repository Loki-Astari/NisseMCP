#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseMCPConfig.h"
#include "Session.h"

#include "Server.h"
#include "MCPCore.h"

#include <chrono>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{
    struct MCPServerConfig
    {
        std::string_view        allowedOrigin;
        std::string_view        slot            = "/mpc";
        Protocol                minProtocol     = Protocol::v2025_11_25;
        Protocol                maxProtocol     = Protocol::v2025_11_25;
        Duration                sessionTimeout  = std::chrono::minutes{30};
        Duration                initHandShake   = std::chrono::seconds{10};
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
        Protocol            defaultProtocol;
        MCPCore             core;
        MCPJanitor          janitor;
        std::string         allowedOrigin;

        public:
            MCPServer(MCPServerConfig const& config, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079});

            virtual JsonRPCCore& getCore() override;
        private:
                    void                removeSession(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response);
            virtual void                handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) override;
                    Session&            validateRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, std::string_view originAllowed);
                    std::string_view    getMethodName(ThorsAnvil::Nisse::HTTP::Request const& request) const;
    };
}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPServer.source"
#endif

#endif
