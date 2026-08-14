#ifndef THORSANVIL_NISSE_MCP_COMMAND_PING_H
#define THORSANVIL_NISSE_MCP_COMMAND_PING_H

#include "NisseMCPConfig.h"
#include "MCPRequestCommon.h"
#include <string>

// https://modelcontextprotocol.io/specification/2025-11-25/schema#ping
namespace ThorsAnvil::Nisse::MCP::JsonRPC
{

/*
 * Used by the ClientMCP to understand what to send
 */
struct Ping
{
    static constexpr char const* method = "ping";

    using Param     = std::optional<int>;       // No parameter. It is just simpler to have an optional parameter that is not set.
    using Result    = Object;
};

}

#endif
