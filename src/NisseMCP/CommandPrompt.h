#ifndef THORSANVIL_NISSE_MCP_COMMAND_PROMPT_H
#define THORSANVIL_NISSE_MCP_COMMAND_PROMPT_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include "CommandContent.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#prompts/get
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#getpromptrequestparams
struct GetPromptRequestParamsArgs
{
    // [key: string]: string };
};
using OptGetPromptRequestParamsArgs = std::optional<GetPromptRequestParamsArgs>;
struct GetPromptRequestParams
{
    OptMeta             /*vera*/    _meta;
    std::string                     name;
    OptGetPromptRequestParamsArgs   arguments;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#getpromptrequest
struct GetPromptRequest
{
    std::string                     jsonrpc; // “2.0”;
    RequestId                       id;
    std::string                     method; // “prompts/get”;
    GetPromptRequestParams          params;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#promptmessage
struct ListPromptsRequest
{
    std::string                     jsonrpc; // “2.0”;
    RequestId                       id;
    OptPaginatedRequestParams       params;
    std::string                     method; // “prompts/list”;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#promptargument
struct PromptArgument
{
    std::string                     name;
    OptString                       title;
    OptString                       description;
    OptBool                         required;
};
using VecPromptArgument             = std::vector<PromptArgument>;
using OptVecPromptArgument          = std::optional<VecPromptArgument>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#prompt
struct Prompt
{
    OptVecIcon                      icons;
    std::string                     name;
    OptString                       title;
    OptString                       description;
    OptVecPromptArgument            arguments;
    OptMeta             /*vera*/    _meta;
};
using VecPrompt                     = std::vector<Prompt>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#listpromptsresult
struct ListPromptsResult
{
    OptMeta             /*vera*/    _meta;
    OptString                       nextCursor;
    VecPrompt                       prompts;
    // [key: string]: unknown;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#promptmessage
struct PromptMessage
{
    Role                            role;
    ContentBlock                    content;
};
using VecPromptMessage              = std::vector<PromptMessage>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#getpromptresult
struct GetPromptResult
{
    OptMeta             /*vera*/    _meta;
    OptString                       description;
    VecPromptMessage                messages;
    // [key: string]: unknown;
};

}

#endif
