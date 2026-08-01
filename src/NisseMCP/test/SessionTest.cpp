#include <gtest/gtest.h>
#include "CommandInitialize.h"
#include "Context.h"
#include "MCPServer.h"
#include "NisseHTTP/Util.h"
#include "NisseServer/Server.h"
#include "NisseHTTP/ClientHTTP.h"
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::Nisse::MCP;

namespace
{

struct MCPServerTest: public MCPServer
{
    public:
        MCPServerTest(ProtocolRange protocolInfo = {Protocol::v2025_11_25, Protocol::v2024_11_05})
            : MCPServer{{.serverName = "SessionTest Server 1.0", .allowedOrigin = "https://thors-anvil.com", .slot = "/mcp", .protocolInfo = protocolInfo}}
        {
            std::cerr << "MCPServerTest: Initialized\n";
        }
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

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{"2.0", 1, "initialize", Command::InitializeRequestParams{}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_11_25, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeProtocolBelowRange)
{
    MCPServerRunner     server{ProtocolRange{Protocol::v2025_03_26, Protocol::v2025_11_25}};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{"2.0", 1, "initialize", Command::InitializeRequestParams{.protocolVersion = Protocol::v2024_11_05}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_03_26, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeProtocolAboveRange)
{
    MCPServerRunner     server{ProtocolRange{Protocol::v2025_03_26, Protocol::v2025_11_25}};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{"2.0", 1, "initialize", Command::InitializeRequestParams{.protocolVersion = Protocol::v2026_07_28}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_11_25, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeProtocolInRange)
{
    MCPServerRunner     server{ProtocolRange{Protocol::v2025_03_26, Protocol::v2025_11_25}};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{"2.0", 1, "initialize", Command::InitializeRequestParams{.protocolVersion = Protocol::v2025_06_18}});
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_06_18, result.result.value().protocolVersion);
}

TEST(SessionTest, InitializeShouldHaveSessionIDSet)
{
    MCPServerRunner     server{ProtocolRange{Protocol::v2025_11_25, Protocol::v2025_11_25}};

    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    // Command::InitializeResponse result = client.post<Command::InitializeResponse>({.path="/mcp", .headers = headers}, Command::InitializeRequest{"2.0", 1, "initialize", Command::InitializeRequestParams{.protocolVersion = Protocol::v2025_06_18}});
    //
    auto const& src = Command::InitializeRequest{"2.0", 1, "initialize", Command::InitializeRequestParams{.protocolVersion = Protocol::v2025_11_25}};
    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path="/mcp", .headers=headers}, ThorsAnvil::Serialize::jsonStreamSize(src), [&src](std::ostream& output)
    {
        output << ThorsAnvil::Serialize::jsonExporter(src, ThorsAnvil::Serialize::OutputType::Stream);
    });
    Command::InitializeResponse result;
    bool called = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        called = true;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(result);
        EXPECT_NE(0, resp.getHeader().getHeader("mcp-session-id").size());

        std::cerr << "HEADERS: " << resp.getHeader() << "\n\n";

    });

    EXPECT_TRUE(called);
    EXPECT_EQ("2.0", result.jsonrpc);
    EXPECT_EQ(1, std::get<long>(result.id));
    EXPECT_FALSE(result.error.has_value());
    ASSERT_TRUE(result.result.has_value());
    EXPECT_EQ("SessionTest Server 1.0", result.result.value().serverInfo.name);
    EXPECT_EQ(Protocol::v2025_11_25, result.result.value().protocolVersion);
}

