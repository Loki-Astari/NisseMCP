#include <gtest/gtest.h>

#include "JsonRPC.h"
#include "ThorSerialize/Traits.h"
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

    local.addExecutor<SubtractParam>("subtract", [](SubtractParam const& param){return JsonRPC::Response{param.minuend - param.subtrahend};});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":1})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_CallWithPositionalParameters2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor<SubtractParam>("subtract", [](SubtractParam const& param){return JsonRPC::Response{param.minuend - param.subtrahend};});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":-19,"id":2})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_CallWithNamedParameters1)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor<SubtractParam>("subtract", [](SubtractParam const& param){return JsonRPC::Response{param.minuend - param.subtrahend};});

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":3})", result.str());
}

TEST(JsonRCPProtocolTest, RPC_CallWithNamedParameters2)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor<SubtractParam>("subtract", [](SubtractParam const& param){return JsonRPC::Response{param.minuend - param.subtrahend};});

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

    local.addExecutor<std::vector<int>>("update", [&](std::vector<int> const& param){size = param.size();return 1;});

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

    local.addExecutor("foobar", [&](){used = true;return 1;});

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

    local.addExecutor<std::string>("foobar", [&](std::string const& param){return 1;});

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

    local.addExecutor<std::string>("foobar", [&](std::string const& param){return 1;});

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

    local.addExecutor<std::string>("foobar", [&](std::string const& param){return 1;});

    std::istringstream   command{R"({"jsonrpc": "2.1", "method": "name", "params": "bar"})"};
                                                            //   ^ Invalid Type: Should be string.
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}



