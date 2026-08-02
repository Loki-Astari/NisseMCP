#include "MCPServer.h"
#include "NisseHTTP/Util.h"
#include <boost/uuid.hpp>
#include <tuple>

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
ThorsAnvil::Nisse::HTTP::Response& MCPServerContext::addHeaders(ThorsAnvil::Nisse::HTTP::Response& response)
{
    return ServerContext::addHeaders(response).addHeader("MCP-Session-Id", dynamic_cast<MCPSession&>(session).toString());
}

NISSEMCP_HEADER_ONLY_INCLUDE
MCPServer::MCPServer(MCPServerConfig const& config, std::size_t workerCount, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit, ThorsAnvil::ThorsSocket::ServerInit&& controlInit)
    : Server{config.slot, workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
    , protocolInfo{config.protocolInfo}
    , janitor{sessionMap, config.sessionTimeout, config.initHandShake}
    , serverName{config.serverName}
    , allowedOrigin{config.allowedOrigin}
{
    addPath(ThorsAnvil::Nisse::HTTP::Method::DELETER, std::string{config.slot}, [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    {
        removeSession(request, response);
        return true;
    });
    addPath(ThorsAnvil::Nisse::HTTP::Method::POST,    std::string{config.slot}, [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    {
        handleRequest(request, response);
        return true;
    });
    addTimer(config.janitorCheckTime, janitor);
}

NISSEMCP_HEADER_ONLY_INCLUDE
JsonRPCCore& MCPServer::getCore()
{
    return core;
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPServer::removeSession(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& /*response*/)
{
    auto const&         sessionHeaders  = request.headers().getHeader("MCP-Session-Id");
    if (sessionHeaders.size() == 0) {
        ThorsLogError("ThorsAnvil::Nisse::MCP::MCPServer", "removeSession", "Attempt to remove session but no session id");
        return;
    }
    std::string_view    sessionHeader   = sessionHeaders[0];
    boost::uuids::uuid  sessionId       = boost::uuids::string_generator{}(std::begin(sessionHeader), std::end(sessionHeader));
    sessionMap.erase(sessionId);
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPServer::handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    MCPSession& session = validateRequest(request, response, allowedOrigin);
    if (!session.isValid())
    {
        return;
    }
    MCPServerContext    context{session, request, response};
    core.handleInputStream(context);
}

NISSEMCP_HEADER_ONLY_INCLUDE
std::string_view MCPServer::getMethodName(ThorsAnvil::Nisse::HTTP::Request const& request) const
{
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
                return methodName;
            }
        }
    }
    return ""sv;
}

NISSEMCP_HEADER_ONLY_INCLUDE
MCPSession& MCPServer::validateRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, std::string_view originAllowed)
{
    static MCPSession notFoundSession{[&](){return serverName;}, [&](){return protocolInfo;}, SessionState::Invalid, Protocol::v2024_11_05, {}};

    // Valid Request must have Origin.
    auto const& origin = request.headers().getHeader("origin");

    if (origin.size() != 1 || origin[0] != originAllowed) {
        response.setStatus(403)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{10, "Invalid Origin. Request forbidden", {}});
        return notFoundSession;
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
        return notFoundSession;
    }

    // Valid request must have a session ID
    // OR: Be an initialize request (in which case the request is to generate a sessionID).
    // IF the session ID has not been confirmed by the client with an notifications/initialized command then its not valid.
    using namespace std::string_view_literals;
    // TODO: Should not need to make that lower-case manually.
    auto const& sessionHeaders = request.headers().getHeader("MCP-Session-Id");

    // No Header with Session-ID check if this is an initialize command.
    if (sessionHeaders.size() == 0 && getMethodName(request) == "initialize"sv)
    {
        boost::uuids::uuid  sessionId       = boost::uuids::random_generator{}();
        auto                newSession      = sessionMap.try_emplace(sessionId, [&](){return serverName;}, [&](){return protocolInfo;}, SessionState::Requested, protocolInfo.second, sessionId);
        return newSession.first->second;
    }

    // If we have multiple Session-ID then give up.
    if (sessionHeaders.size() != 1)
    {
        response.setStatus(400)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{12, "Invalid or missing Session Id", {}});
        return notFoundSession;
    }

    // Try and find the session ID
    std::string_view    sessionHeader   = sessionHeaders[0];
    auto                find            = std::end(sessionMap);
    try
    {
        boost::uuids::uuid  sessionId = boost::uuids::string_generator{}(std::begin(sessionHeader), std::end(sessionHeader));
        find = sessionMap.find(sessionId);
    }
    catch (...) {/* Ignore Error as find will not be changed and the next test will indicate failure */}

    if (find == std::end(sessionMap)) {
        // Session Not Found.
        response.setStatus(404)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{12, "Invalid or missing Session Id", {}});
        return notFoundSession;
    }
    MCPSession&            session         = find->second;
    if (session.isRequested())
    {
        std::string_view methodName = getMethodName(request);
        if (methodName != "notifications/initialized"sv && methodName != "ping"sv) {
            // Session is only requested and has not been confirmed by the client.
            // We will ignore until the client has correctly initialized.
            response.setStatus(404)
                    .addHeader("content-type", "application/json")
                    .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                    << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{12, "Invalid or missing Session Id", {}});
            return notFoundSession;
        }
    }

    // Check protocol:
    bool protocolMismatch = false;
    auto const& protocols = request.headers().getHeader("MCP-Protocol-Version");
    if (protocols.size() != 1) {
        // Header does not have a protocol.
        protocolMismatch = true;
    }
    else {
        Protocol protocol = ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::getValue(protocols[0], "");
        if (session.getProtocol() != protocol) {
            // header protocol does not match session protocol.
            protocolMismatch = true;
        }
    }
    if (protocolMismatch) {
        response.setStatus(400)
                .addHeader("content-type", "application/json")
                .body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked)
                << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{13, "Protocol Session Mismatch", {}});
        return notFoundSession;
    }

    return session;
}
