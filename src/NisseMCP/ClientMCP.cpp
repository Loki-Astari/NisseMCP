#include "ClientMCP.h"

#include "MCPRequestInitialize.h"

using namespace ThorsAnvil::Nisse::MCP;

struct GetHost
{
    std::string_view    operator()(ThorsAnvil::ThorsSocket::FileInfo const& /*info*/)   {ThorsLogAndThrowFatal(std::runtime_error, "ThorsAnvil::Nisse::MCP::GetHost", "operator()", "Trying to get host from a file!!");}
    std::string_view    operator()(ThorsAnvil::ThorsSocket::PipeInfo const& /*info*/)   {ThorsLogAndThrowFatal(std::runtime_error, "ThorsAnvil::Nisse::MCP::GetHost", "operator()", "Trying to get host from a pipe!!");}
    std::string_view    operator()(ThorsAnvil::ThorsSocket::SocketInfo const& info)     {return info.host;}
    std::string_view    operator()(ThorsAnvil::ThorsSocket::SocketService const& info)  {return info.host;}
    std::string_view    operator()(ThorsAnvil::ThorsSocket::SSocketInfo const& info)    {return info.host;}
    std::string_view    operator()(ThorsAnvil::ThorsSocket::SSocketService const& info) {return info.host;}
};

NISSEMCP_HEADER_ONLY_INCLUDE
ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketInfo const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientMCP(Init::Constructor, config, socketInfo, version)
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInfo const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientMCP(Init::Constructor, config, socketInfo, version)
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketService const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientMCP(Init::Constructor, config, socketInfo, version)
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketService const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientMCP(Init::Constructor, config, socketInfo, version)
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientMCP::ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInit const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version)
    : ClientMCP(Init::Constructor, config, socketInfo, version)
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientMCP::ClientMCP(Init,
        ClientConfig const& config,
        ThorsAnvil::ThorsSocket::SocketInit const& socketInfo,
        ThorsAnvil::Nisse::HTTP::Version version)
    : config{config}
    , pool{socketInfo, version}
    , requestId{1}
{
    std::string_view host = std::visit(GetHost{}, socketInfo);

    headers.add("origin", host);
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    ConnectionFromPool  client{pool};

    client->post_async(ThorsAnvil::Nisse::HTTP::ClientRequest{.path=std::string{config.slot}, .headers=headers}, JsonRPC::InitializeRequest{getNextRequestId(), {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        JsonRPC::InitializeResponse reply;
        if (resp.getStatus() != 202 || !(resp.body() >> ThorsAnvil::Serialize::jsonImporter(reply))) {
            ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: Bad response from server");
        }

        auto const& sessionIds      = resp.getHeader().getHeader("MCP-Session-Id");
        if (sessionIds.size() != 1) {
            ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: No Session Information from server");
        }

        if (!reply.result.has_value()) {
            ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: No Protocol Information from server");
        }

        Protocol returnedProtocol = reply.result.value().protocolVersion;
        if (returnedProtocol != config.protocol) {
            ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: Requested protocol not supported");
        }

        headers.add("MCP-Session-Id", sessionIds[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(config.protocol));
    });

    client->post_async({.path=std::string{config.slot}, .headers=headers}, JsonRPC::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        if (resp.getStatus() != 200) {
            ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::MCP::ClientMCP", "init", "Failed to initialize MCP server connection: Handshake was not accepted");
        }
    });
}

NISSEMCP_HEADER_ONLY_INCLUDE
void ClientMCP::logErrorMessage(int status, int code, std::string_view message)
{
    ThorsLogError("ThorsAnvil::Nisse::MCP::ClientMCP", "logErrorMessage", "HTTP Status: ", status, " JsonRPC Error Code: ", code, " Message: ", message);
}
