#ifndef THORSANVIL_NISSE_MCP_COMMAND_TASK_H
#define THORSANVIL_NISSE_MCP_COMMAND_TASK_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#tasks
// https://modelcontextprotocol.io/specification/2025-11-25/schema#tasks/get
// https://modelcontextprotocol.io/specification/2025-11-25/schema#tasks/result
// https://modelcontextprotocol.io/specification/2025-11-25/schema#tasks/list
// https://modelcontextprotocol.io/specification/2025-11-25/schema#tasks/cancel
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#relatedtaskmetadata
struct RelatedTaskMetadata
{
    std::string                 taskId;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#taskstatus
enum class TaskStatus {working, input_required, completed, failed, cancelled};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#task
using TTL                       = std::unique_ptr<Number>;
struct Task
{
    std::string                 taskId;
    TaskStatus                  status;
    OptString                   statusMessage;
    std::string                 createdAt;
    std::string                 lastUpdatedAt;
    TTL                         ttl;
    OptNumber                   pollInterval;
};
using VecTask                   = std::vector<Task>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#taskmetadata
struct TaskMetadata
{
    OptNumber                   ttl;
};
using OptTaskMetadata           = std::optional<TaskMetadata>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#createtaskresult
struct CreateTaskResult
{
    OptMeta         /*vera*/    _meta;
    Task                        task;
    // [key: string]: unknown;
};

struct TaskRequestParam
{
    std::string                 taskId;
};
// https://modelcontextprotocol.io/specification/2025-11-25/schema#gettaskrequest
struct GetTaskRequest
{
    std::string                 jsonrpc; // “2.0”;
    RequestId                   id;
    std::string                 method; // “tasks/get”;
    TaskRequestParam            params;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#gettaskresult
using GetTaskResult             = std::variant<Result, Task>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#gettaskpayloadrequest
struct GetTaskPayloadRequest
{
    std::string                 jsonrpc; // “2.0”;
    RequestId                   id;
    std::string                 method; // “tasks/result”;
    TaskRequestParam            params;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#gettaskpayloadresult
struct GetTaskPayloadResult
{
    OptMeta         /*vera*/    _meta;
    // [key: string]: unknown;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listtasksrequest
struct ListTasksRequest
{
    std::string                 jsonrpc; // “2.0”;
    RequestId                   id;
    OptPaginatedRequestParams   params;
    std::string                 method; // “tasks/list”;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listtasksresult
struct ListTasksResult
{
    OptMeta         /*vera*/    _meta;
    OptString                   nextCursor;
    VecTask                     tasks;
    // [key: string]: unknown;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#canceltaskrequest
struct CancelTaskRequest
{
    std::string                 jsonrpc; // “2.0”;
    RequestId                   id;
    std::string                 method; // “tasks/cancel”;
    TaskRequestParam            params;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#canceltaskresult
using CancelTaskResult          = std::variant<Result, Task>;

}

#endif
