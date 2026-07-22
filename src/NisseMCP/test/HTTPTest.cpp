#include <gtest/gtest.h>

#include "Server.h"
#include "MCPCore.h"
#include "NisseHTTP/ClientHTTP.h"

using namespace ThorsAnvil::Nisse::MCP;
using MCPServer = Server<MCPCore>;

class MCPTestServer: public MCPServer
{
    public:
        MCPTestServer(MCPCore& core)
            : MCPServer{core, 1, ThorsAnvil::ThorsSocket::ServerInfo{8080}, ThorsAnvil::ThorsSocket::ServerInfo{8070}}
        {
            addPath(ThorsAnvil::Nisse::HTTP::Method::GET, "/mcp", [](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
            {
                response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked) << "\"Morning\"";
                return true;    // Indicates we handeled the request. Don't search for more matches.
            });
        }
};

using MCPServerRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<MCPTestServer>;

TEST(HTTPTest, ServerRun)
{
    MCPCore             core{Protocol::v2025_11_25};
    MCPServerRunner     server{core};

    ThorsAnvil::Nisse::HTTP::ClientHTTP client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8080}};
    std::string reply = client.get<std::string>({.path = "/mcp"});

    EXPECT_EQ("Morning", reply);

#if 0
                EXPECT_EQ(resp.getStatus(), 200);
                EXPECT_EQ(resp.getMessage(), "OK");
                ASSERT_TRUE(resp.getHeader().hasHeader("transfer-encoding"sv));
                EXPECT_EQ(resp.getHeader().getHeader("transfer-encoding")[0], "chunked");
    using namespace std::string_literals;
    std::string line = runner.sendMessage("A message"s);
    std::cerr << "Reply:<\n" << line << "\n=======\n";
    EXPECT_EQ("Morning", line);
#endif
}
