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
        int                                 id;
        bool                                termNeeded;
        ThorsAnvil::Nisse::HTTP::Response&  response;
        std::ostream*                       body;
        std::map<std::string, std::string>  headers;
        int                                 status;
        public:
            ServerContext(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
                : Context{request.body()}
                , id{1}
                , termNeeded{false}
                , response{response}
                , body{nullptr}
                , status{200}
            {}
            ~ServerContext()
            {
                termPreviousItem();
            }
            void termPreviousItem()
            {
                if (termNeeded) {
                    getBody() << "\r\n\r\n";
                    termNeeded = false;
                }
            }
            std::ostream& getBody()
            {
                if (body == nullptr) {
                    response.setStatus(status);
                    for (auto const& header: headers) {
                        response.addHeader(header.first, header.second);
                    }
                    body = &response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked);
                }
                return *body;
            }

            virtual void serverSideStream() override
            {
                status = 202;
                headers.insert_or_assign("content-type", "text/event-stream");
                Context::serverSideStream();
            }
            virtual void error(int code, std::string_view message) override
            {
                if (body == nullptr && !stream) {
                    status = 404;
                    headers.insert_or_assign("content-type", "application/json");
                    getBody();
                }
                Context::error(code, message);
            }
            virtual void stop() const override
            {}

            virtual void addNote() override
            {
                if (body == nullptr) {
                    status = 202;
                    getBody();
                }
            }
            virtual std::ostream& addItem() override
            {
                if (body == nullptr && !stream) {
                    status = 202;
                    headers.insert_or_assign("content-type", "application/json");
                }
                termPreviousItem();
                if (stream) {
                    getBody() << "id: " << id << "\r\n"
                           << "data: ";
                    ++id;
                    termNeeded = true;
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
