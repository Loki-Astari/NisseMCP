#include "Server.h"
#include "Session.h"

using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
ServerContext::ServerContext(Session& session, ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    : Context{session, request.body()}
    , response{response}
    , body{nullptr}
    , status{200}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ServerContext::~ServerContext()
{
    if (stream && count > 0) {
        (*body) << "\r\n\r\n";
    }
    else if (body == nullptr) {
        response.setStatus(status);
    }
}

NISSEMCP_HEADER_ONLY_INCLUDE
void ServerContext::serverSideStream()
{
    status = 202;
    Context::serverSideStream();
}

NISSEMCP_HEADER_ONLY_INCLUDE
void ServerContext::error(JsonRPC::OptRequestId id, int code, std::string_view message)
{
    if (!stream) {
        status = 400;
    }
    Context::error(id, code, message);
}

NISSEMCP_HEADER_ONLY_INCLUDE
void ServerContext::addNote()
{
    if (!stream) {
        status = 202;
    }
}

NISSEMCP_HEADER_ONLY_INCLUDE
std::ostream& ServerContext::addItem()
{
    if (!stream && status == 200) {
        // Note: the error() call above will set status = 400 and then call Context:error() which will then call
        //       addItem() to get a stream. Thus we should not set the status if error has already set the status.
        status = 202;
    }
    if (body == nullptr) {
        std::ostream& s = addHeaders(response.setStatus(status))
                            .addHeader("content-type", stream ? "text/event-stream" : "application/json")
                            .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked);
        body = &s;
    }
    if (stream) {
        (*body) << ((count > 0) ? "\r\n\r\n" : "")
                << "id: " << (count + 1) << "\r\n"
                << "data: ";
    }
    return (*body);
}

NISSEMCP_HEADER_ONLY_INCLUDE
Server::Server(std::string_view slot, std::size_t workerCount, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit, ThorsAnvil::ThorsSocket::ServerInit&& controlInit)
    : ThorsAnvil::Nisse::HTTP::Server{workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
{
    addPath(ThorsAnvil::Nisse::HTTP::Method::POST, std::string{slot}, [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    {
        handleRequest(request, response);
        return true;
    });
}

NISSEMCP_HEADER_ONLY_INCLUDE
Server::~Server()
{}
