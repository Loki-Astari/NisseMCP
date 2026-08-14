#ifndef THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H
#define THORSANVIL_NISSE_MCP_COMMAND_LOGGING_H

#include "NisseMCPConfig.h"
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
struct SetLevelRequestUserParam
{
    LoggingLevel    level;
};
struct SetLevelRequest
{
    std::string             jsonrpc; // “2.0”;
    SerializableId          id;
    std::string             method; // “logging/setLevel”;
    SetLevelRequestParams   params;
    SetLevelRequest(SerializableId&& id, SetLevelRequestUserParam const& param)
        : jsonrpc{"2.0"}
        , id{std::move(id)}
        , method{"logging/setLevel"}
        , params{param.level}
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

struct LoggingResponse
{
    std::string                     jsonrpc;
    SerializableId                  id;
    OptObject                       result;
    JsonRPC::OptError               error;
};

struct Logging
{
    using Request       = SetLevelRequest;
    using UserParam     = SetLevelRequestUserParam;
    using Result        = LoggingResponse;
    using UserData      = Object;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingMessageNotificationParams, level, logger, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingMessageNotification,       jsonrpc, method, params);

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::SetLevelRequestParams,            level);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::SetLevelRequest,                  jsonrpc, id, method, params);

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingResponse,                  jsonrpc, id, result, error);

#endif
