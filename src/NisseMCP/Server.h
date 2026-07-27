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
        ThorsAnvil::Nisse::HTTP::Response&  response;
        std::ostream*                       body;
        int                                 status;
        public:
            ServerContext(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
                : Context{request.body()}
                , response{response}
                , body{nullptr}
                , status{200}
            {}
            ~ServerContext()
            {
                if (stream && count > 0) {
                    (*body) << "\r\n\r\n";
                }
                else if (body == nullptr) {
                    response.setStatus(status);
                }
            }
            std::ostream& getBody()
            {
                if (body == nullptr) {
                    std::ostream& s = response.setStatus(status)
                                              .addHeader("content-type", stream ? "text/event-stream" : "application/json")
                                              .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked);
                    body = &s;
                }
                return *body;
            }

            virtual void serverSideStream() override
            {
                status = 202;
                Context::serverSideStream();
            }
            virtual void error(JsonRPC::OptRequestId id, int code, std::string_view message) override
            {
                if (body == nullptr && !stream) {
                    status = 400;
                    getBody();
                }
                Context::error(id, code, message);
            }

            virtual void addNote() override
            {
                if (body == nullptr) {
                    status = 202;
                }
            }
            virtual std::ostream& addItem() override
            {
                if (body == nullptr && !stream) {
                    status = 202;
                }
                if (stream && count > 0) {
                    (*body) << "\r\n\r\n";
                }
                if (stream) {
                    getBody() << "id: " << (count + 1) << "\r\n"
                              << "data: ";
                }
                return getBody();
            }
    };
    template<typename Core, typename RequestValidator = typename Core::DefaultValidator>
    class Server: public ThorsAnvil::Nisse::HTTP::Server
    {
        Core&               core;
        RequestValidator    validator;
        std::string         allowedOrigin;

        public:
            Server(std::string_view allowedOrigin, Core& core, std::size_t workerCount = 4, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit = ThorsAnvil::ThorsSocket::ServerInfo{8070}, ThorsAnvil::ThorsSocket::ServerInit&& controlInit = ThorsAnvil::ThorsSocket::ServerInfo{8079})
                : ThorsAnvil::Nisse::HTTP::Server{workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
                , core{core}
                , allowedOrigin{allowedOrigin}
            {
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
                ServerContext     context{request, response};
                core.handleInputStream(context);
            }

    };
}

#endif
