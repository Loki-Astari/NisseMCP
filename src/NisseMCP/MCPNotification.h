#ifndef THORSANVIL_NISSE_MCP_NOTIFICATION_H
#define THORSANVIL_NISSE_MCP_NOTIFICATION_H

#include "NisseMCPConfig.h"
#include "LoggingLevel.h"
#include "JsonRPC.h"
#include "MCPRequestCommon.h"
#include "ThorSerialize/Traits.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
namespace ThorsAnvil::Nisse::MCP::JsonRPC
{

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

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingMessageNotificationParams, level, logger, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::LoggingMessageNotification,       jsonrpc, method, params);

#endif
