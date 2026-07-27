#include <gtest/gtest.h>

#include "JsonRPC.h"
#include "Context.h"
#include "JsonRPCCore.h"
#include "NisseHTTP/ClientHTTP.h"
#include "NisseHTTP/Util.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"
#include <numeric>
#include <sstream>


#include "Server.h"

// Examples of expected output from JsonRCP calls.
//    https://www.jsonrpc.org/specification

struct SubtractParam
{
    int     minuend;
    int     subtrahend;
};

using namespace ThorsAnvil::Nisse::MCP;

struct ServerTest: public Server<JsonRPCCore>
{
    JsonRPCCore     core;
    std::size_t     size = 0;
    bool            used = false;
    public:
        ServerTest()
            : Server<JsonRPCCore>{"https://thors-anvil.com", core}
        {
            core.addExecutor("subtract",     [&](Context& context, JsonRPC::OptRequestId id, SubtractParam const& param){context.addItem(id, param.minuend - param.subtrahend);});
            core.addExecutor("update",       [&](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& param){size = param.size();context.addItem(id, 1);});
            core.addExecutor("foobar",       [&](Context& context, JsonRPC::OptRequestId id){used = true;context.addItem(id, 1);});

            core.addExecutor("sum",          [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& args){context.addItem(id, std::accumulate(std::begin(args), std::end(args), 0));});
            core.addExecutor("notify_hello", [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& /*a*/){context.addItem(id, 1);});
            core.addExecutor("get_data",     [](Context& context, JsonRPC::OptRequestId id){std::vector<std::string> result; result.emplace_back("hello"); result.emplace_back("5"); context.addItem(id, result);});
            core.addExecutor("notify_sum",   [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& args){context.addItem(id, std::accumulate(std::begin(args), std::end(args), 0));});
            core.addExecutor("note",         [](Context& context, JsonRPC::OptRequestId id)->int {throw std::runtime_error("Hi");});
            core.addExecutor("Hi",           [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const&){context.addItem(id, "Hi");});
            core.addExecutor("throw",        [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& args)->int {throw std::runtime_error("Checking");});
        }
        std::size_t getSize() const {return size;}
        bool        isUsed()  const {return used;}
};

using ServerTestRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<ServerTest>;

ThorsAnvil_MakeTrait(SubtractParam, minuend, subtrahend);

int sendToMCP(std::istream& command, std::ostream& result, std::string_view type)
{
    ThorsAnvil::Nisse::HTTP::ClientHTTP  client({"localhost", 8070});
    int httpResult = 0;
    client.send(ThorsAnvil::Nisse::HTTP::Method::POST, {.path = "/mcp"}, ThorsAnvil::Nisse::HTTP::Encoding::Chunked, [&](ThorsAnvil::Nisse::HTTP::StreamOutput& action)
    {
        action << command.rdbuf();
    });
    client.processResp([&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
    {
        result << resp.body().rdbuf();
        httpResult = resp.getStatus();
        auto const& ctype = resp.getHeader().getHeader("content-type");
        if (!type.empty()) {
            ASSERT_EQ(1, ctype.size());
            EXPECT_EQ(type, ctype[0]);
        }
        else {
            ASSERT_EQ(0, ctype.size());
        }
    });
    return httpResult;
}

TEST(JsonServerRPCProtocolTest, RPC_CallWithPositionalParameters1)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":1})", result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_UsingAStringID)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "long-string-that-forms-id"})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":"long-string-that-forms-id"})", result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_UsingAStructureID)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": {"name": "long-string-that-forms-id"}})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_UsingAnArrayID)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": ["name", "long-string-that-forms-id"]})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_UsingABoolID)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": true})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_UsingANullID)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": null})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_UsingAFloatID)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 22.234})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_CallWithPositionalParameters2)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":-19,"id":2})", result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_CallWithNamedParameters1)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":3})", result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_CallWithNamedParameters2)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"minuend": 42, "subtrahend": 23}, "id": 4})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":4})", result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_Notification1)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "");

    EXPECT_EQ(R"()", result.str());
    EXPECT_EQ(5, local.callServer<int>([](ServerTest& server){return server.getSize();}));
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_Notification2)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar"})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "");

    EXPECT_EQ(R"()", result.str());
    EXPECT_TRUE(local.callServer<int>([](ServerTest& server){return server.isUsed();}));
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_NonExistentMethod)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobarbaz", "id": "1"})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"1"})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidJson)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz])"};
                                                                                        //     ^^^^^
                                                                                        // Missing close quote
                                                                                        // Bad close ']' not '}'
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidRequest1)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": 1, "params": "bar"})"};
                                                            //   ^ Invalid Type: Should be string.
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    // This is deteted as PARSE Errors. because the method must be a string.
    // EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidRequest2)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.1", "method": "name", "params": "bar"})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_EmptyBatch)
{
    ServerTestRunner     local;
    std::istringstream   command{R"([])"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidEmptyBatch)
{
    ServerTestRunner     local;
    std::istringstream   command{R"([)"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidRequests1)
{
    ServerTestRunner     local;
    std::istringstream   command{R"([1,2,3])"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "text/event-stream");

    /* Specs say this */
#if 0
    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})"
              R"(])"
                , result.str());
#endif
    // But we check the type of each member on input. This is not a JsonRPC request so we get a parse error.
    // When trying to read the first command.
    // Because we have a parse errors (this is bad JSON so we have to abort any further reading.
    EXPECT_EQ("id: 1\r\n"
              "data: " R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})" "\r\n"
              "\r\n"
                , result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidRequests2)
{
    ServerTestRunner     local;
    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},)"
                                    R"({"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},)"
                                    R"({"foo": "boo"},)"
                                    R"({"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},)"
                                    R"({"jsonrpc": "2.0", "method": "get_data", "id": "9"})"
                                 R"(])"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "text/event-stream");

    EXPECT_EQ("id: 1\r\n"
              "data: " R"({"jsonrpc":"2.0","result":7,"id":"1"})" "\r\n"
              "\r\n"
              "id: 2\r\n"
              "data: " R"({"jsonrpc":"2.0","result":19,"id":"2"})" "\r\n"
              "\r\n"
              "id: 3\r\n"
              "data: " R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})" "\r\n"
              "\r\n"
              "id: 4\r\n"
              "data: " R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"5"})" "\r\n"
              "\r\n"
              "id: 5\r\n"
              "data: " R"({"jsonrpc":"2.0","result":["hello","5"],"id":"9"})" "\r\n"
              "\r\n"
                , result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_InvalidRequestsBADJSONInArray)
{
    ServerTestRunner     local;
    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},)"
                                    R"({"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"})" // Not Missing comma
                                    R"({"foo": "boo"},)"
                                    R"({"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},)"
                                    R"({"jsonrpc": "2.0", "method": "get_data", "id": "9"} )"
                                 R"(])"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "text/event-stream");

    EXPECT_EQ("id: 1\r\n"
              "data: " R"({"jsonrpc":"2.0","result":7,"id":"1"})" "\r\n"
              "\r\n"
              "id: 2\r\n"
              "data: " R"({"jsonrpc":"2.0","result":19,"id":"2"})" "\r\n"
              "\r\n"
              "id: 3\r\n"
              "data: " R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})" "\r\n"
              "\r\n"
                , result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, RPC_BatchNotification)
{
    ServerTestRunner     local;
    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "notify_sum", "params": [1,2,4]},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]})"
                                 R"(])"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "");

    EXPECT_EQ("", result.str());
    EXPECT_EQ(httpResult, 202);
}

TEST(JsonServerRPCProtocolTest, CatchExceptionsOutOfExecutor1)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "throw", "params": [1,2,4], "id": 5})"};
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Internal error"},"id":5})",
            result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, CatchExceptionsOutOfExecutor2)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "note", "id": 5})"};
                                                                                // ^^ Array of string not integer.
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Internal error"},"id":5})",
            result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, CheckForInvalidParameters)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "sum", "params": ["1","2","4"], "id": 5})"};
                                                                                // ^^ Array of string not integer.
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32602,"message":"Invalid params"},"id":5})",
            result.str());
    EXPECT_EQ(httpResult, 400);
}

TEST(JsonServerRPCProtocolTest, CheckForInvalidParametersPassedToFuncThatTakesZero)
{
    ServerTestRunner     local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "Hi", "params": 1, "id": 5})"};
                                                                                // ^^ Array of string not integer.
    std::ostringstream   result;

    int httpResult = sendToMCP(command, result, "application/json");

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32602,"message":"Invalid params"},"id":5})",
            result.str());
    EXPECT_EQ(httpResult, 400);
}
