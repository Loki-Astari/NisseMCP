#ifndef THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H
#define THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include "ThorSerialize/Traits.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
namespace ThorsAnvil::Nisse::MCP::Command
{

enum class LoggingLevel {debug, info, notice, warning, error, critical, alert, emergency };

// https://modelcontextprotocol.io/specification/2025-11-25/schema#setlevelrequestparams
struct SetLevelRequestParams
{
    // OptMeta     /*vera*/    _meta;
    // TODO Meta
    LoggingLevel            level;
    SetLevelRequestParams()
    {}
    SetLevelRequestParams(LoggingLevel level)
        : level{level}
    {}
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#setlevelrequest
struct SetLevelRequest
{
    std::string             jsonrpc; // “2.0”;
    RequestId               id;
    std::string             method; // “logging/setLevel”;
    SetLevelRequestParams   params;
    SetLevelRequest(RequestId&& id, LoggingLevel level)
        : jsonrpc{"2.0"}
        , id{std::move(id)}
        , method{"logging/setLevel"}
        , params{level}
    {}
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
    using Request   = SetLevelRequest;
    using Result    = LoggingMessageNotification;
    using UserData  = LoggingMessageNotificationParams;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::LoggingMessageNotificationParams, level, logger, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::LoggingMessageNotification,       jsonrpc, method, params);

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::SetLevelRequestParams,            level);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::SetLevelRequest,                  jsonrpc, id, method, params);

#endif
