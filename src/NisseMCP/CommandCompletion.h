#ifndef THORSANVIL_NISSE_MCP_COMMAND_COMPLETION_H
#define THORSANVIL_NISSE_MCP_COMMAND_COMPLETION_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"


// https://modelcontextprotocol.io/specification/2025-11-25/schema#completion/complete
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#completeresult
struct Completion
{
    VecString               values;
    OptNumber               total;
    OptBool                 hasMore;
};
struct CompleteResult
{
    OptMeta   /*vera*/      _meta;
    Completion              completion;
    // [key: string]: unknown;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#promptreference
struct PromptReference
{
    std::string             type; // “ref/prompt”;
    std::string             name;
    OptString               title;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#resourcetemplatereference
struct ResourceTemplateReference
{
    std::string             type; // “ref/resource”;
    std::string             uri;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#completerequestparams
using Ref                   = std::variant<PromptReference, ResourceTemplateReference>;
using SimpleMap             = std::map<std::string, std::string>;
using OptSimpleMap          = std::optional<SimpleMap>;

struct CompletionContent
{
    OptSimpleMap            arguments;
};
using OptCompletionContent  = std::optional<CompletionContent>;
struct CompleteRequestParams
{
    OptMeta     /*vera*/    _meta;
    Ref                     ref;
    SimpleMap               argument;
    OptCompletionContent    context;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#completerequest
struct CompleteRequest
{
    std::string             jsonrpc; // “2.0”;
    RequestId               id;
    std::string             method; // “completion/complete”;
    CompleteRequestParams   params;
};

}

#endif
