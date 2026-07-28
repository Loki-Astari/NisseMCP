#include "MCPServer.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
MCPServer::MCPServer(MCPServerConfig const& config, std::size_t workerCount, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit, ThorsAnvil::ThorsSocket::ServerInit&& controlInit)
    : Server{config.slot, workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
    , core{config.protocol}
    , allowedOrigin{config.allowedOrigin}
{
    addPath(ThorsAnvil::Nisse::HTTP::Method::POST, "/mcp", [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    {
        handleRequest(request, response);
        return true;
    });
}

NISSEMCP_HEADER_ONLY_INCLUDE
JsonRPCCore& MCPServer::getCore()
{
    return core;
}


NISSEMCP_HEADER_ONLY_INCLUDE
void MCPServer::handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    if (!validateRequest(request, response, allowedOrigin)) {
        // Validation has already set the response code and sent appropriate output to the stream;
        request.body().setstate(std::ios_base::failbit);
        return;
    }
    ServerContext     context{request, response};
    core.handleInputStream(context);
}

NISSEMCP_HEADER_ONLY_INCLUDE
bool MCPServer::validateRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, std::string_view originAllowed)
{
    // Valid Request must have Origin.
    auto const& origin = request.headers().getHeader("origin");

    if (origin.size() != 1 || origin[0] != originAllowed) {
        response.setStatus(403)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{10, "Invalid Origin. Request forbidden", {}});
        return false;
    }

    // Valid request must indicate that client accepts json or SSE stream.
    auto const& accept = request.headers().getHeader("accept");
    bool acceptJson = false;
    bool acceptStream= false;
    for (auto const& aVal: accept) {
        if (aVal == "application/json") {
            acceptJson = true;
        }
        if (aVal == "text/event-stream") {
            acceptStream = true;
        }
    }
    if (!acceptJson || !acceptStream) {
        response.setStatus(404)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{11, "Invalid Accept: Requires 'application/json' and 'text/event-stream'", {}});
        return false;
    }

    // Valid request must have a session ID
    // OR: Be an initialize request (in which case the request is to generate a sessionID).
    auto const& seassion = request.headers().getHeader("MCP-Session-Id");
    bool sessionOK = false;
    if (seassion.size() == 1) {
        sessionOK = true;
    }
    else
    {
        if (seassion.size() == 0)
        {
            // If there is no seassion ID then see if this request contains:  "method": "initialize"
            // This is a basic check. We will do full validation when we unpack the JSON but this should suffice for here.
            using namespace std::string_view_literals;
            auto view = request.preloadStreamIntoBuffer(true);
            auto method = view.find(R"("method")"sv);
            if (method != std::string_view::npos) {
                method = view.find_first_not_of(" \r\n\v\t"sv, method + 8);
                if (method != std::string_view::npos && view[method] == ':') {
                    method = view.find_first_not_of(" \r\n\v\t", method + 1);
                    if (method != std::string_view::npos && view[method] == '"') {
                        auto methodEnd = view.find('"', method + 1);
                        std::string_view methodName = view.substr(method + 1, methodEnd - method - 1);
                        if (methodName == "initialize"sv) {
                            sessionOK = true;
                        }
                    }
                }
            }
        }
    }
    if (!sessionOK) {
        response.setStatus(400)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{12, "Invalid or missing Session Id", {}});
        return false;
    }

    // All validation checks passed.
    return true;
}
