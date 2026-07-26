#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseMCPConfig.h"

#include "JsonRPC.h"
#include "MCPCore.h"
#include "Context.h"
#include "CommandInitialize.h"

#include "NisseHTTP/Util.h"
#include "NisseHTTP/Server.h"
#include "ThorSerialize/JsonThor.h"

#include <string>

namespace ThorsAnvil::Nisse::MCP
{
    class ServerContext: public Context
    {
        int     id;
        bool    termNeeded;
        public:
            ServerContext(std::istream& input, std::ostream& output)
                : Context{input, output}
                , id{1}
                , termNeeded{false}
            {}
            ~ServerContext()
            {
                termPreviousItem();
            }
            void termPreviousItem()
            {
                if (termNeeded) {
                    output << "\r\n\r\n";
                    termNeeded = false;
                }
            }

            virtual void stop() const override
            {}

            virtual std::ostream& addItem() override
            {
                termPreviousItem();
                if (stream) {
                    output << "id: " << id << "\r\n"
                           << "data: ";
                    ++id;
                    termNeeded = true;
                }
                return output;
            }
    };
    template<typename Core, typename RequestValidator = typename Core::DefaultValidator>
    class Server: public ThorsAnvil::Nisse::HTTP::Server
    {
        Core&               core;
        RequestValidator    validator;
        std::string         allowedOrigin;

        ThorsAnvil::Nisse::HTTP::HeaderResponse headers;
        public:
            Server(std::string_view allowedOrigin, Core& core, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079})
                : ThorsAnvil::Nisse::HTTP::Server{workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
                , core{core}
                , allowedOrigin{allowedOrigin}
            {
                headers.add("content-type", "application/json"); // text/event-stream
                addPath(ThorsAnvil::Nisse::HTTP::Method::POST, "/mcp", [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
                {
                    handleRequest(request, response);
                    return true;
                });
            }

            void handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
            {
                if (!validator.validateRequest(request, response, allowedOrigin)) {
                    // Validation has already set the response code and sent appropriate output to the stream;
                    return;
                }
                ServerContext     context{request.body(), response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)};
                response.setStatus(core.handleInputStream(context) ? 202 : 404);
            }

    };
}

#endif
