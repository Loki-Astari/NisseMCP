#ifndef THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H
#define THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H

#include "NisseMCPConfig.h"
#include "LoggingLevel.h"
#include "JsonRPC.h"
#include "MCPRequestCommon.h"
#include "ThorSerialize/Traits.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
namespace ThorsAnvil::Nisse::MCP::JsonRPC
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#setlevelrequest
struct SetLevelRequestParam
{
    LoggingLevel    level;
};

struct Logging
{
    static constexpr char const* method = "logging/setLevel";
    using Param         = SetLevelRequestParam;
    using Result        = Object;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::SetLevelRequestParam,             level);

#endif
