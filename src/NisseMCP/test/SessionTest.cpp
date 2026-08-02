#include <gtest/gtest.h>
#include <chrono>
#include "CommandInitialize.h"
#include "CommandPing.h"
#include "CommandLogging.h"
#include "Context.h"
#include "MCPServer.h"
#include "NisseHTTP/Util.h"
#include "NisseServer/Server.h"
#include "NisseHTTP/ClientHTTP.h"
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::Nisse::MCP;

namespace
{

MCPServerConfig defaultConfig = {.serverName = "SessionTest Server 1.0", .allowedOrigin = "https://thors-anvil.com", .slot = "/mcp", .protocolInfo = {Protocol::v2025_11_25, Protocol::v2024_11_05}};
struct MCPServerTest: public MCPServer
{
    public:
        MCPServerTest(MCPServerConfig&& config = {.serverName = "SessionTest Server 1.0", .allowedOrigin = "https://thors-anvil.com", .slot = "/mcp", .protocolInfo = {Protocol::v2025_11_25, Protocol::v2024_11_05}})
            : MCPServer{std::move(config)}
        {}
};

using MCPServerRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<MCPServerTest>;

}

TEST(SessionTest, InitializeOnly)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{1, {}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_11_25, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeProtocolBelowRange)
{
    MCPServerConfig     config{defaultConfig};
    config.protocolInfo = {Protocol::v2025_03_26, Protocol::v2025_11_25};
    MCPServerRunner     server{std::move(config)};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2024_11_05}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_03_26, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeProtocolAboveRange)
{
    MCPServerConfig     config{defaultConfig};
    config.protocolInfo = {Protocol::v2025_03_26, Protocol::v2025_11_25};
    MCPServerRunner     server{std::move(config)};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2026_07_28}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_11_25, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeProtocolInRange)
{
    MCPServerConfig     config{defaultConfig};
    config.protocolInfo = {Protocol::v2025_03_26, Protocol::v2025_11_25};
    MCPServerRunner     server{std::move(config)};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_06_18}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_06_18, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeShouldHaveSessionIDSet)
{
    MCPServerConfig     config{defaultConfig};
    config.protocolInfo = {Protocol::v2025_03_26, Protocol::v2025_11_25};
    MCPServerRunner     server{std::move(config)};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    bool called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(1, resp.getHeader().getHeader("mcp-session-id").size());
    });

    EXPECT_TRUE(called);
}

TEST(SessionTest, SendPingWithNoSessionId)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    bool called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::PingRequest{1}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(400, resp.getStatus());
    });
    EXPECT_TRUE(called);
}

TEST(SessionTest, SendPingAfterHandShake)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse initResponse;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(initResponse);

        ASSERT_EQ(202, resp.getStatus());
        headers.add("MCP-Session-Id", resp.getHeader().getHeader("mcp-session-id")[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(initResponse.result.value().protocolVersion));
    });

    client.post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        ASSERT_EQ(200, resp.getStatus());
    });

    bool called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::PingRequest{1}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(202, resp.getStatus());
    });
    EXPECT_TRUE(called);
}

TEST(SessionTest, SendInitNotificationToSlowly)
{
    // Janitor runs every 10 seconds.
    // So have to wait a min 10 seconds for the sweep.
    MCPServerConfig     config{defaultConfig};
    config.janitorCheckTime = std::chrono::seconds{1};
    config.initHandShake    = std::chrono::seconds{1};

    MCPServerRunner     server{std::move(config)};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse initResponse;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(initResponse);

        ASSERT_EQ(202, resp.getStatus());
        headers.add("MCP-Session-Id", resp.getHeader().getHeader("mcp-session-id")[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(initResponse.result.value().protocolVersion));
    });
    sleep(2);

    client.post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        ASSERT_EQ(404, resp.getStatus());
    });
}

TEST(SessionTest, SendPingAfterSessionDelete)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse initResponse;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(initResponse);

        ASSERT_EQ(202, resp.getStatus());
        headers.add("MCP-Session-Id", resp.getHeader().getHeader("mcp-session-id")[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(initResponse.result.value().protocolVersion));
    });

    client.post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        ASSERT_EQ(200, resp.getStatus());
    });

    client.send(ThorsAnvil::Nisse::HTTP::Method::DELETER, {.path="/mcp", .headers=headers}, 0, [](ThorsAnvil::Nisse::HTTP::StreamOutput& action) {});
    bool called = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(200, resp.getStatus());
    });
    EXPECT_TRUE(called);
    called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::PingRequest{2}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(404, resp.getStatus());
    });
    EXPECT_TRUE(called);
}

TEST(SessionTest, SendPingBeforeHandshakeCompleteisOK)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse initResponse;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(initResponse);

        ASSERT_EQ(202, resp.getStatus());
        headers.add("MCP-Session-Id", resp.getHeader().getHeader("mcp-session-id")[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(initResponse.result.value().protocolVersion));
    });

    bool called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::PingRequest{2}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(202, resp.getStatus());
    });
    EXPECT_TRUE(called);
    called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        ASSERT_EQ(200, resp.getStatus());
    });
    EXPECT_TRUE(called);

}

TEST(SessionTest, SendLoggingBeforeHandshakeCompleteisIsNotOK)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse initResponse;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(initResponse);

        ASSERT_EQ(202, resp.getStatus());
        headers.add("MCP-Session-Id", resp.getHeader().getHeader("mcp-session-id")[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(initResponse.result.value().protocolVersion));
    });

    bool called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::SetLevelRequest{2, Command::LoggingLevel::alert}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(404, resp.getStatus());
    });
    EXPECT_TRUE(called);
    called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        ASSERT_EQ(200, resp.getStatus());
    });
    EXPECT_TRUE(called);

}

TEST(SessionTest, SendLoggingAfterHandshakeCompleteisIsOK)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.post_async({.path="/mcp", .headers=headers}, Command::InitializeRequest{1, {.protocolVersion = Protocol::v2025_11_25}}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        Command::InitializeResponse initResponse;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(initResponse);

        ASSERT_EQ(202, resp.getStatus());
        headers.add("MCP-Session-Id", resp.getHeader().getHeader("mcp-session-id")[0]);
        headers.add("MCP-Protocol-Version", ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::to_string(initResponse.result.value().protocolVersion));
    });

    bool called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::Notification_Initialized{}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        ASSERT_EQ(200, resp.getStatus());
    });
    EXPECT_TRUE(called);
    called = false;
    client.post_async({.path="/mcp", .headers=headers}, Command::SetLevelRequest{2, Command::LoggingLevel::alert}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        EXPECT_EQ(202, resp.getStatus());
    });
    EXPECT_TRUE(called);

}

