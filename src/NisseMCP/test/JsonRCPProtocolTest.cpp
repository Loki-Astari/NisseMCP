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
