#ifndef THORSANVIL_NISSE_MCP_COMMAND_PING_H
#define THORSANVIL_NISSE_MCP_COMMAND_PING_H

#include "NisseMCPConfig.h"
#include "MCPRequestCommon.h"
#include <string>

// https://modelcontextprotocol.io/specification/2025-11-25/schema#ping
namespace ThorsAnvil::Nisse::MCP::JsonRPC
{

struct PingResponse
{
    std::string                     jsonrpc;
    SerializableId                  id;
    OptObject                       result;
    JsonRPC::OptError               error;
};

/*
 * Used by the ClientMCP to understand what to send
 */
struct Ping
{
    static constexpr char const* method = "ping";

    using Param     = std::optional<int>;
    using Result    = PingResponse;
    using UserData  = Object;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::PingResponse,        jsonrpc, id, result, error);

#endif
