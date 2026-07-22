#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseHTTP/Util.h"
#include "NisseMCPConfig.h"
#include "MCPCore.h"
#include "Context.h"
#include "CommandInitialize.h"

#include "NisseHTTP/Server.h"

namespace ThorsAnvil::Nisse::MCP
{
    class ServerContext: public Context
    {
        public:
            ServerContext(std::istream& input, std::ostream& output, Protocol protocol)
                : Context(input, output, protocol)
            {}
            ~ServerContext()
            {}

            virtual void stop() const override
            {}

            virtual std::ostream& addItem() override
            {
                return output;
            }
    };
    class Server: public MCPCore, public ThorsAnvil::Nisse::HTTP::Server
    {
        public:
            Server(MCPCoreConfig const& config, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079})
                : MCPCore{config}
                , ThorsAnvil::Nisse::HTTP::Server{workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
            {
                addPath(ThorsAnvil::Nisse::HTTP::Method::POST, "/mcp", [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
                {
                    ServerContext     context{request.body(), response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked), Protocol::v2025_11_25};
                    handleInputStream(context);
                    return true;
                });
            }

    };
}

#endif
