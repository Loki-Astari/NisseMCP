#include <gtest/gtest.h>

#include "JsonRPC.h"
#include "Context.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"
#include <numeric>
#include <sstream>


#include "Local.h"

// Examples of expected output from JsonRCP calls.
//    https://www.jsonrpc.org/specification

struct SubtractParam
{
    int     minuend;
    int     subtrahend;
};

ThorsAnvil_MakeTrait(SubtractParam, minuend, subtrahend);

using namespace ThorsAnvil::Nisse::MCP;

TEST(JsonRCPProtocolTest, RPC_CallWithPositionalParameters1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":1})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_UsingAStringID)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "long-string-that-forms-id"})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":"long-string-that-forms-id"})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_UsingAStructureID)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": {"name": "long-string-that-forms-id"}})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_UsingAnArrayID)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": ["name", "long-string-that-forms-id"]})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_UsingABoolID)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": true})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_UsingANullID)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": null})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_UsingAFloatID)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 22.234})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_CallWithPositionalParameters2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":-19,"id":2})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_CallWithNamedParameters1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":3})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_CallWithNamedParameters2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"minuend": 42, "subtrahend": 23}, "id": 4})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":4})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_Notification1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    std::size_t                             size = 0;

    local.addExecutor("update", [&](Context& context, std::vector<int> const& param){size = param.size();context.addItem(1);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"()", result.str());
    EXPECT_EQ(5, size);
}

TEST(JsonRCPProtocolTest, RPC_Notification2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    bool                                    used = false;

    local.addExecutor("foobar", [&](Context& context){used = true;context.addItem(1);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar"})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"()", result.str());
    EXPECT_TRUE(used);
}

TEST(JsonRCPProtocolTest, RPC_NonExistentMethod)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar", "id": "1"})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"1"})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidJson)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("foobar", [&](Context& context, std::string const& param){context.addItem(1);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz])"};
                                                                                        //     ^^^^^
                                                                                        // Missing close quote
                                                                                        // Bad close ']' not '}'
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidRequest1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("foobar", [&](Context& context, std::string const& param){context.addItem(1);});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": 1, "params": "bar"})"};
                                                            //   ^ Invalid Type: Should be string.
    std::ostringstream   result;

    local.run(command, result);

    // This is deteted as PARSE Errors. because the method must be a string.
    // EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidRequest2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("foobar", [&](Context& context, std::string const& param){context.addItem(1);});

    std::istringstream   command{R"({"jsonrpc": "2.1", "method": "name", "params": "bar"})"};
                                                            //   ^ Invalid Type: Should be string.
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_EmptyBatch)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    std::istringstream   command{R"([])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidEmptyBatch)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    std::istringstream   command{R"([)"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidRequests1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    std::istringstream   command{R"([1,2,3])"};
    std::ostringstream   result;

    local.run(command, result);

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
    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})"
              R"(])"
                , result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidRequests2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("sum",          [](Context& context, std::vector<int> const& args){context.addItem(std::accumulate(std::begin(args), std::end(args), 0));});
    local.addExecutor("subtract",     [](Context& context, std::vector<int> const& args){context.addItem(args[0] - args[1]);});
    local.addExecutor("notify_hello", [](Context& context, std::vector<int> const& /*a*/){context.addItem(1);});
    local.addExecutor("get_data",     [](Context& context){std::vector<std::string> result; result.emplace_back("hello"); result.emplace_back("5"); context.addItem(result);});

    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},)"
                                    R"({"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},)"
                                    R"({"foo": "boo"},)"
                                    R"({"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},)"
                                    R"({"jsonrpc": "2.0", "method": "get_data", "id": "9"} )"
                                 R"(])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","result":7,"id":"1"},)"
                R"({"jsonrpc":"2.0","result":19,"id":"2"},)"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null},)"
                R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"5"},)"
                R"({"jsonrpc":"2.0","result":["hello","5"],"id":"9"})"
              R"(])"
                , result.str());
}

TEST(JsonRCPProtocolTest, RPC_InvalidRequestsBADJSONInArray)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("sum",          [](Context& context, std::vector<int> const& args){context.addItem(std::accumulate(std::begin(args), std::end(args), 0));});
    local.addExecutor("subtract",     [](Context& context, std::vector<int> const& args){context.addItem(args[0] - args[1]);});
    local.addExecutor("notify_hello", [](Context& context, std::vector<int> const& /*a*/){context.addItem(1);});
    local.addExecutor("get_data",     [](Context& context){std::vector<std::string> result; result.emplace_back("hello"); result.emplace_back("5"); context.addItem(result);});

    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},)"
                                    R"({"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"})" // Not Missing comma
                                    R"({"foo": "boo"},)"
                                    R"({"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},)"
                                    R"({"jsonrpc": "2.0", "method": "get_data", "id": "9"} )"
                                 R"(])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","result":7,"id":"1"},)"
                R"({"jsonrpc":"2.0","result":19,"id":"2"},)"
                R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})"
              R"(])"
                , result.str());
}

TEST(JsonRCPProtocolTest, RPC_BatchNotification)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("notify_sum",   [](Context& context, std::vector<int> const& args){context.addItem(std::accumulate(std::begin(args), std::end(args), 0));});
    local.addExecutor("notify_hello", [](Context& context, std::vector<int> const& /*a*/){context.addItem(1);});

    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "notify_sum", "params": [1,2,4]},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]})"
                                 R"(])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ("", result.str());
}

TEST(JsonRCPProtocolTest, CatchExceptionsOutOfExecutor1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("sum",   [](Context& context, std::vector<int> const& args)->int {throw std::runtime_error("Checking");});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": 5})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Internal error"},"id":5})",
            result.str());
}

TEST(JsonRCPProtocolTest, CatchExceptionsOutOfExecutor2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("note",   [](Context& context)->int {throw std::runtime_error("Hi");});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "note", "id": 5})"};
                                                                                // ^^ Array of string not integer.
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Internal error"},"id":5})",
            result.str());
}

TEST(JsonRCPProtocolTest, CheckForInvalidParameters)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("sum",          [](Context& context, std::vector<int> const& args){context.addItem(std::accumulate(std::begin(args), std::end(args), 0));});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "sum", "params": ["1","2","4"], "id": 5})"};
                                                                                // ^^ Array of string not integer.
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32602,"message":"Invalid params"},"id":5})",
            result.str());
}

TEST(JsonRCPProtocolTest, CheckForInvalidParametersPassedToFuncThatTakesZero)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("note",   [](Context& context, std::vector<int> const&){context.addItem("Hi");});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "note", "params": 1, "id": 5})"};
                                                                                // ^^ Array of string not integer.
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32602,"message":"Invalid params"},"id":5})",
            result.str());
}
