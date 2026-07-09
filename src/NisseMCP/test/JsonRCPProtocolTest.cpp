#include <gtest/gtest.h>
#include <sstream>


#include "Local.h"

// Examples of expected output from JsonRCP calls.
//    https://www.jsonrpc.org/specification
//
TEST(JsonRCPProtocolTest, RPC_CallWithNamedParameters)
{
    ThorsAnvil::Nisse::MCP::ServerConfig    config;
    ThorsAnvil::Nisse::MCP::Local           local{config};

    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})"};
    std::ostringstream   result;

    local.run(command, result);

    EXPECT_EQ(R"({"jsonrpc": "2.0", "result": 19, "id": 3})", result.str());
}
