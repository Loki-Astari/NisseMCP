#include <gtest/gtest.h>
#include "CommandInitialize.h"
#include "Context.h"
#include "MCPServer.h"
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

// enum class Protocol {v2024_11_05, v2025_03_26, v2025_06_18, v2025_11_25, v2026_07_28};
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

