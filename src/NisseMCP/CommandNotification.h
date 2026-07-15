#ifndef THORSANVIL_NISSE_MCP_COMMAND_NOTIFICATION_H
#define THORSANVIL_NISSE_MCP_COMMAND_NOTIFICATION_H

#include "CommandCommon.h"
#include "CommandTask.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/cancelled
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/initialized
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/tasks/status
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/message
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/progress
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/prompts/list_changed
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/resources/list_changed
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/resources/updated
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/roots/list_changed
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/tools/list_changed
// https://modelcontextprotocol.io/specification/2025-11-25/schema#notifications/elicitation/complete
namespace ThorsAnvil::Nisse::MCP::Command
{
    struct NotificationParams
    {
    };
    using OptNotificationParams         = std::optional<NotificationParams>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#cancellednotificationparams
    struct CancelledNotificationParams
    {
        OptMeta             /*vera*/    _meta;
        OptRequestId                    requestId;
        OptString                       reason;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#cancellednotification
    struct CancelledNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/cancelled”;
        CancelledNotificationParams     params;
    };


    // https://modelcontextprotocol.io/specification/2025-11-25/schema#initializednotification
    struct InitializedNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/initialized”;
        OptNotificationParams           params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#taskstatusnotificationparams
    using TaskStatusNotificationParams  = std::variant<NotificationParams, Task>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#taskstatusnotification
    struct TaskStatusNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/tasks/status”;
        TaskStatusNotificationParams    params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#loggingmessagenotificationparams
    struct LoggingMessageNotificationParams
    {
        OptMeta             /*vera*/    _meta;
        LoggingLevel                    level;
        OptString                       logger;
        // data: unknown;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#loggingmessagenotification
    struct LoggingMessageNotification
    {
        std::string                         jsonrpc; // “2.0”;
        std::string                         method; // “notifications/message”;
        LoggingMessageNotificationParams    params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#progressnotificationparams
    struct ProgressNotificationParams
    {
        OptMeta             /*vera*/    _meta;
        ProgressToken                   progressToken;
        Number                          progress;
        OptNumber                       total;
        OptString                       message;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#progressnotification
    struct ProgressNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/progress”;
        ProgressNotificationParams      params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#promptlistchangednotification
    struct PromptListChangedNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/prompts/list_changed”;
        OptNotificationParams           params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#resourcelistchangednotification
    struct ResourceListChangedNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/resources/list_changed”;
        OptNotificationParams           params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#resourceupdatednotificationparams
    struct ResourceUpdatedNotificationParams
    {
        OptMeta             /*vera*/    _meta;
        std::string                     uri;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#resourceupdatednotification
    struct ResourceUpdatedNotification
    {
        std::string                         jsonrpc; // “2.0”;
        std::string                         method; // “notifications/resources/updated”;
        ResourceUpdatedNotificationParams   params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#rootslistchangednotification
    struct RootsListChangedNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; // “notifications/roots/list_changed”;
        OptNotificationParams           params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#toollistchangednotification
    struct ToolListChangedNotification
    {
        std::string                     jsonrpc; // “2.0”;
        std::string                     method; //  “notifications/tools/list_changed”;
        OptNotificationParams           params;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#elicitationcompletenotification
    struct ElicitationCompleteNotificationParam
    {
        std::string                     elicitationId;
    };
    struct ElicitationCompleteNotification
    {
        std::string                             jsonrpc; // “2.0”;
        std::string                             method; // “notifications/elicitation/complete”;
        ElicitationCompleteNotificationParam    params;
    };
}

#endif
