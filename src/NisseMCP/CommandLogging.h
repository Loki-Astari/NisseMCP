#ifndef THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H
#define THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
namespace ThorsAnvil::Nisse::MCP::Command
{
    // https://modelcontextprotocol.io/specification/2025-11-25/schema#setlevelrequestparams
    struct SetLevelRequestParams
    {
        OptMeta     /*vera*/    _meta;
        LoggingLevel            level;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#setlevelrequest
    struct SetLevelRequest
    {
        std::string             jsonrpc; // “2.0”;
        RequestId               id;
        std::string             method; // “logging/setLevel”;
        SetLevelRequestParams   params;
    };
}

#endif
