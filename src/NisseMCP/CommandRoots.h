#ifndef THORSANVIL_NISSE_MCP_COMMAND_ROOTS_H
#define THORSANVIL_NISSE_MCP_COMMAND_ROOTS_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#roots/list
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listrootsrequest
struct ListRootsRequest
{
    std::string             jsonrpc; // “2.0”;
    RequestId               id;
    std::string             method; // “roots/list”;
    OptRequestParams        params;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#root
struct Root
{
    std::string             uri;
    OptString               name;
    OptMeta     /*vera*/    _meta;
};
using VecRoot               = std::vector<Root>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listrootsresult
struct ListRootsResult
{
    OptMeta     /*vera*/    _meta;
    VecRoot                 roots;
    // [key: string]: unknown;
};

}

#endif
