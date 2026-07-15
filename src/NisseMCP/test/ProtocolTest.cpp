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

TEST(Protocol, protocol_v2024_11_05)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config{Protocol::v2024_11_05};
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"([{"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"([{"jsonrpc":"2.0","result":19,"id":1}])", result.str());
}

TEST(Protocol, protocol_v2025_03_26)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config{Protocol::v2025_03_26};
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"([{"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"([{"jsonrpc":"2.0","result":19,"id":1}])", result.str());
}

TEST(Protocol, protocol_v2025_06_18)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config{Protocol::v2025_06_18};
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"([{"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(Protocol, protocol_v2025_11_25)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config{Protocol::v2025_11_25};
    ThorsAnvil::Nisse::MCP::Local           local{config};

    local.addExecutor("subtract", [](Context& context, SubtractParam const& param){context.addItem(param.minuend - param.subtrahend);});

    std::istringstream   command{R"([{"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1}])"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

