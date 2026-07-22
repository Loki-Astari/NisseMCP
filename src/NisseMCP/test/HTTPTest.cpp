#include <gtest/gtest.h>

#include "Context.h"
#include "NisseHTTP/Util.h"
#include "Server.h"
#include "MCPCore.h"
#include "NisseHTTP/ClientHTTP.h"
#include "CommandInitialize.h"
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::Nisse::MCP;
using MCPServer = Server<MCPCore>;

struct MCPServerTest: public Server<MCPCore>
{
    MCPCore         core;
    public:
        MCPServerTest(Protocol protocol = Protocol::v2025_11_25)
            : Server<MCPCore>{core}
            , core{protocol}
        {}
};

using MCPServerRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<MCPServerTest>;

TEST(HTTPTest, ServerRun)
{
    MCPServerRunner     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    Command::InitializeRequest   request{.jsonrpc = "2.0", .id = 1, .method = "initialize", .params = {.protocolVersion = "2025_11_25"}};
    // Command::InitializeResult    reply = client.post<Command::InitializeResult>({.path = "/mcp"}, request);
    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp"}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& out)
    {
        out << ThorsAnvil::Serialize::jsonExporter(request, Context::outputConfig);
        return true;
    });
    client.processResp([](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        std::cerr << "Status:  " << resp.getStatus() << "\n"
                  << "Message: " << resp.getMessage() << "\n"
                  << "Version: " << resp.getVersion() << "\n"
                  << "Header:  " << resp.getHeader() << "\n"
                  << "Body:   >" << resp.body().rdbuf() << "<\n";
    });


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
