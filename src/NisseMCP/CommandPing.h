#ifndef THORSANVIL_NISSE_MCP_COMMAND_PING_H
#define THORSANVIL_NISSE_MCP_COMMAND_PING_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#ping
namespace ThorsAnvil::Nisse::MCP::Command
{
    // https://modelcontextprotocol.io/specification/2025-11-25/schema#pingrequest
    struct PingRequest
    {
        std::string                     jsonrpc; // “2.0”;
        RequestId                       id;
        std::string                     method; // “ping”;
        OptRequestParams                params;
    };

}

#endif
