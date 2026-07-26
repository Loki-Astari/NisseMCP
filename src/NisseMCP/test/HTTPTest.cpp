#include <gtest/gtest.h>

#include "Context.h"
#include "JsonRPC.h"
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
            : Server<MCPCore>{"https://thors-anvil.com", core}
            , core{protocol}
        {}
};

using MCPServerRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<MCPServerTest>;

TEST(HTTPTest, AcceptValidRequest)
{
    MCPServerRunner                         server;
    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp", .headers = headers}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& out)
    {
        out << ThorsAnvil::Serialize::jsonExporter(Command::InitializeRequest{.jsonrpc = "2.0", .id = 1, .method = "initialize", .params = {.protocolVersion = "2025_11_25"}}, Context::outputConfig);
        return true;
    });
    bool responseProcessed = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        responseProcessed = true;
        ASSERT_EQ(202, resp.getStatus());
    });
    EXPECT_TRUE(responseProcessed);
}
TEST(HTTPTest, AcceptValidRequestSingleAccept)
{
    MCPServerRunner                         server;
    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "application/json,text/event-stream");

    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp", .headers = headers}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& out)
    {
        out << ThorsAnvil::Serialize::jsonExporter(Command::InitializeRequest{.jsonrpc = "2.0", .id = 1, .method = "initialize", .params = {.protocolVersion = "2025_11_25"}}, Context::outputConfig);
        return true;
    });
    bool responseProcessed = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        responseProcessed = true;
        ASSERT_EQ(202, resp.getStatus());
    });
    EXPECT_TRUE(responseProcessed);
}
TEST(HTTPTest, NoOriginProvided)
{
    MCPServerRunner                     server;
    ThorsAnvil::Nisse::HTTP::ClientHTTP client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("accept", "application/json");
    headers.add("accept", "text/event-stream");

    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp", .headers = headers}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& out)
    {
        out << ThorsAnvil::Serialize::jsonExporter(Command::InitializeRequest{.jsonrpc = "2.0", .id = 1, .method = "initialize", .params = {.protocolVersion = "2025_11_25"}}, Context::outputConfig);
        return true;
    });
    bool responseProcessed = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        responseProcessed = true;
        ASSERT_EQ(403, resp.getStatus());
        ASSERT_EQ("Forbidden", resp.getMessage());

        JsonRPC::ClientResponse errorValue;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(errorValue);

        ASSERT_TRUE(errorValue.error.has_value());
        EXPECT_EQ(10, errorValue.error.value().code);
    });
    EXPECT_TRUE(responseProcessed);
}

TEST(HTTPTest, NotAcceptJson)
{
    MCPServerRunner                         server;
    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "text/event-stream");

    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp", .headers = headers}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& out)
    {
        out << ThorsAnvil::Serialize::jsonExporter(Command::InitializeRequest{.jsonrpc = "2.0", .id = 1, .method = "initialize", .params = {.protocolVersion = "2025_11_25"}}, Context::outputConfig);
        return true;
    });
    bool responseProcessed = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        responseProcessed = true;
        ASSERT_EQ(404, resp.getStatus());

        JsonRPC::ClientResponse errorValue;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(errorValue);

        ASSERT_TRUE(errorValue.error.has_value());
        EXPECT_EQ(11, errorValue.error.value().code);
    });
    EXPECT_TRUE(responseProcessed);
}

TEST(HTTPTest, NotAcceptStream)
{
    MCPServerRunner                         server;
    ThorsAnvil::Nisse::HTTP::ClientHTTP     client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8070}};
    ThorsAnvil::Nisse::HTTP::HeaderRequest  headers;
    headers.add("origin", "https://thors-anvil.com");
    headers.add("accept", "text/event-stream");

    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp", .headers = headers}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& out)
    {
        out << ThorsAnvil::Serialize::jsonExporter(Command::InitializeRequest{.jsonrpc = "2.0", .id = 1, .method = "initialize", .params = {.protocolVersion = "2025_11_25"}}, Context::outputConfig);
        return true;
    });
    bool responseProcessed = false;
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        responseProcessed = true;
        ASSERT_EQ(404, resp.getStatus());

        JsonRPC::ClientResponse errorValue;
        resp.body() >> ThorsAnvil::Serialize::jsonImporter(errorValue);

        ASSERT_TRUE(errorValue.error.has_value());
        EXPECT_EQ(11, errorValue.error.value().code);
    });
    EXPECT_TRUE(responseProcessed);
}

