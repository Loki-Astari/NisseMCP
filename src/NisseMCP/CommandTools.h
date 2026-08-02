#ifndef THORSANVIL_NISSE_MCP_COMMAND_TOOLS_H
#define THORSANVIL_NISSE_MCP_COMMAND_TOOLS_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include "CommandTask.h"
#include "CommandContent.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#tools/call
// https://modelcontextprotocol.io/specification/2025-11-25/schema#tools/list
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#calltoolrequestparams
struct CallToolRequestParams
{
    OptTaskMetadata             task;
    OptMeta         /*vera*/    _meta;
    std::string                 name;
    // arguments?: { [key: string]: unknown };
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#calltoolrequest
struct CallToolRequest
{
    std::string                 jsonrpc; // “2.0”;
    RequestId                   id;
    std::string                 method; // “tools/call”;
    CallToolRequestParams       params;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#calltoolresult
struct CallToolResult
{
    OptMeta         /*vera*/    _meta;
    VecContentBlock             content;
    // structuredContent?: { [key: string]: unknown };
    OptBool                     isError;
    // [key: string]: unknown;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listtoolsrequest
struct ListToolsRequest
{
    std::string                 jsonrpc; // “2.0”;
    RequestId                   id;
    OptPaginatedRequestParams   params;
    std::string                 method; // “tools/list”;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#toolannotations
struct ToolAnnotations
{
    OptString                   title;
    OptBool                     readOnlyHint;
    OptBool                     destructiveHint;
    OptBool                     idempotentHint;
    OptBool                     openWorldHint;
};
using OptToolAnnotations        = std::optional<ToolAnnotations>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#toolexecution
enum class TaskSupport {forbidden, optional, required};
using OptTaskSupport            = std::optional<TaskSupport>;
struct ToolExecution
{
    OptTaskSupport              taskSupport;
};
using OptToolExecution          = std::optional<ToolExecution>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#tool
struct Tool
{
    OptVecIcon                  icons;
    std::string                 name;
    OptString                   title;
    OptString                   description;
    Schema                      inputSchema;
    OptToolExecution            execution;
    Schema                      outputSchema;
    OptToolAnnotations          annotations;
    OptMeta         /*vera*/    _meta;
};
using VecTool                   = std::vector<Tool>;
using OptVecTool                = std::optional<VecTool>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listtoolsresult
struct ListToolsResult
{
    OptMeta         /*vera*/    _meta;
    OptString                   nextCursor;
    VecTool                     tools;
    // [key: string]: unknown;
};

}

#endif
