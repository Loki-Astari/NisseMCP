#ifndef THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H
#define THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"
#include "MCPRequestCommon.h"
#include "ThorSerialize/Traits.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
namespace ThorsAnvil::Nisse::MCP::JsonRPC
{

enum class LoggingLevel
{
    emergency       = ThorsAnvil::ThorsVerbosity::Emergency,
    alert           = ThorsAnvil::ThorsVerbosity::Alert,
    critical        = ThorsAnvil::ThorsVerbosity::Critical,
    error           = ThorsAnvil::ThorsVerbosity::Error,
    warning         = ThorsAnvil::ThorsVerbosity::Warning,
    notice          = ThorsAnvil::ThorsVerbosity::Notice,
    info            = ThorsAnvil::ThorsVerbosity::Info,
    debug           = ThorsAnvil::ThorsVerbosity::Debug,
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#setlevelrequest
struct SetLevelRequestParam
{
    LoggingLevel    level;
};
// https://modelcontextprotocol.io/specification/2025-11-25/schema#loggingmessagenotificationparams
struct LoggingMessageNotificationParams
{
    //OptMeta             /*vera*/    _meta;
    // TODO Meta
    LoggingLevel                    level;
    OptString                       logger;
    using JsonBlock = ThorsAnvil::Serialize::AnyBlock;
    JsonBlock                       data;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#loggingmessagenotification
struct LoggingMessageNotification
{
    std::string                         jsonrpc; // “2.0”;
    std::string                         method; // “notifications/message”;
    LoggingMessageNotificationParams    params;
};

struct Logging
{
    static constexpr char const* method = "logging/setLevel";
    using Param         = SetLevelRequestParam;
    using Result        = Object;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingMessageNotificationParams, level, logger, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingMessageNotification,       jsonrpc, method, params);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::SetLevelRequestParam,             level);

#endif
