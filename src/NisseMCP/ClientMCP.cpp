#include "ClientMCP.h"

#include "CommandInitialize.h"

using namespace ThorsAnvil::Nisse::MCP;

ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketInfo const& info, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientHTTP{info, version}
    , protocol{config.protocol}
{
    init(config, info.host);
}

ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInfo const& info, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientHTTP{info, version}
    , protocol{config.protocol}
{
    init(config, info.host);
}

ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketService const& info, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientHTTP{info, version}
    , protocol{config.protocol}
{
    init(config, info.host);
}

ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketService const& info, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientHTTP{info, version}
    , protocol{config.protocol}
{
    init(config, info.host);
}

void ClientMCP::init(ClientConfig const& config, std::string_view origin)
{
    slot        = config.slot;
    requestId   = 1;

    headers.add("origin", origin);
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");
    post_async(ThorsAnvil::Nisse::HTTP::ClientRequest{.path=config.slot, .headers=headers}, Command::InitializeRequest{getNextRequestId(), {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse reply;
        if (resp.getStatus() != 202 || !(resp.body() >> ThorsAnvil::Serialize::jsonImporter(reply))) {
            ThorsLogAndThrowInfo(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: Bad response from server");
        }

        auto const& sessionIds      = resp.getHeader().getHeader("MCP-Session-Id");
        if (sessionIds.size() != 1) {
            ThorsLogAndThrowInfo(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: No Session Information from server");
        }

        if (!reply.result.has_value()) {
            ThorsLogAndThrowInfo(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: No Protocol Information from server");
        }

        Protocol returnedProtocol = reply.result.value().protocolVersion;
        if (returnedProtocol != protocol) {
            ThorsLogAndThrowInfo(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: Requested protocol not supported");
        }

        headers.add("MCP-Session-Id", sessionIds[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(protocol));
    });

    post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        if (resp.getStatus() != 200) {
            ThorsLogAndThrowInfo(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: Handshake was not accepted");
        }
    });
}

bool ClientMCP::resetStream()
{
    return ClientHTTP::resetStream();
}

void ClientMCP::logErrorMessage(int status, int code, std::string_view message)
{
    ThorsLogInfo("ThorsAnvil::Nisse::MCP::ClientMCP", "logErrorMessage", "HTTP Status: ", status, " JsonRPC Error Code: ", code, " Message: ", message);
}
