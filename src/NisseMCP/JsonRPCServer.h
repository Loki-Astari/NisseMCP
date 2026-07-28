#ifndef THORSANVIL_NISSE_MCP_JSONRPC_SERVER_H
#define THORSANVIL_NISSE_MCP_JSONRPC_SERVER_H

#include "NisseMCPConfig.h"

#include "Server.h"
#include "JsonRPCCore.h"

namespace ThorsAnvil::Nisse::MCP
{
    class JsonRPCServer: public Server
    {
        JsonRPCCore         core;

        public:
            JsonRPCServer(std::string_view slot, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079})
                : Server{slot, workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
            {}

            virtual JsonRPCCore& getCore() override {return core;}
        private:
            virtual void handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response) override
            {
                ServerContext     context{request, response};
                core.handleInputStream(context);
            }

    };
}

#endif
