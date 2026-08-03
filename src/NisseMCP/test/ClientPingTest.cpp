#include <gtest/gtest.h>

#include "MCPServer.h"
#include "ClientMCP.h"
#include "CommandPing.h"

using namespace ThorsAnvil::Nisse::MCP;

namespace
{

MCPServerConfig defaultConfig = {.serverName = "SessionTest Server 1.0", .allowedOrigin = "https://thors-anvil.com", .slot = "/mcp", .protocolInfo = {Protocol::v2024_11_05, Protocol::v2025_11_25}};
struct MCPServerTest: public MCPServer
{
    public:
        MCPServerTest(MCPServerConfig&& config = {.serverName = "SessionTest Server 1.0", .allowedOrigin = "localhost", .slot = "/mcp", .protocolInfo = {Protocol::v2024_11_05, Protocol::v2025_11_25}})
            : MCPServer{std::move(config)}
        {}
};

using MCPServerRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<MCPServerTest>;

}



TEST(ClientPingTest, SendPing)
{
    MCPServerRunner     server;

    ClientMCP           client{{.protocol = Protocol::v2025_11_25}, {"localhost", 8070}};

    bool pingResponse = false;
    client.sendRequest<Command::Ping>([&](Command::Object&&){pingResponse = true;});
    ASSERT_TRUE(pingResponse);
}
