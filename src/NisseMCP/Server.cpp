#include "Server.h"

using namespace ThorsAnvil::Nisse::MCP;

ServerContext::ServerContext(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    : Context{request.body()}
    , response{response}
    , body{nullptr}
    , status{200}
{}

ServerContext::~ServerContext()
{
    if (stream && count > 0) {
        (*body) << "\r\n\r\n";
    }
    else if (body == nullptr) {
        response.setStatus(status);
    }
}

void ServerContext::serverSideStream()
{
    status = 202;
    Context::serverSideStream();
}

void ServerContext::error(JsonRPC::OptRequestId id, int code, std::string_view message)
{
    if (!stream) {
        status = 400;
    }
    Context::error(id, code, message);
}

void ServerContext::addNote()
{
    if (!stream) {
        status = 202;
    }
}

std::ostream& ServerContext::addItem()
{
    if (!stream && status == 200) {
        // Note: the error() call above will set status = 400 and then call Context:error() which will then call
        //       addItem() to get a stream. Thus we should not set the status if error has already set the status.
        status = 202;
    }
    if (body == nullptr) {
        std::ostream& s = response.setStatus(status)
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
