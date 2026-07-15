#ifndef THORSANVIL_NISSE_MCP_COMMAND_SAMPLING_H
#define THORSANVIL_NISSE_MCP_COMMAND_SAMPLING_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include "CommandContent.h"
#include "CommandTask.h"
#include "CommandTools.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#sampling/createmessage
namespace ThorsAnvil::Nisse::MCP::Command
{
    // https://modelcontextprotocol.io/specification/2025-11-25/schema#modelhint
    struct ModelHint
    {
        OptString                   name;
    };
    using VecModelHint              = std::vector<ModelHint>;
    using OptVecModelHint           = std::optional<VecModelHint>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#modelpreferences
    struct ModelPreferences
    {
        OptVecModelHint             hints;
        OptNumber                   costPriority;
        OptNumber                   speedPriority;
        OptNumber                   intelligencePriority;
    };
    using OptModelPreferences       = std::optional<ModelPreferences>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#toolusecontent
    struct ToolUseContent
    {
        std::string                 type; // “tool_use”;
        std::string                 id;
        std::string                 name;
        // input: { [key: string]: unknown };
        OptMeta         /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#toolresultcontent
    struct ToolResultContent
    {
        std::string                 type; // “tool_result”;
        std::string                 toolUseId;
        VecContentBlock             content;
        // structuredContent?: { [key: string]: unknown };
        OptBool                     isError;
        OptMeta         /*vera*/    _meta;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#samplingmessagecontentblock
    using SamplingMessageContentBlock = std::variant<TextContent, ImageContent, AudioContent, ToolUseContent, ToolResultContent>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#createmessageresult
    using VecSamplingMessageContentBlock = std::vector<SamplingMessageContentBlock>;
    using CreateMessageResultSample = std::variant<SamplingMessageContentBlock, VecSamplingMessageContentBlock>;
    struct CreateMessageResult
    {
        OptMeta         /*vera*/    _meta;
        std::string                 model;
        OptString                   stopReason;
        Role                        role;
        CreateMessageResultSample   content;
        // [key: string]: unknown;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#samplingmessage
    struct SamplingMessage
    {
        Role                        role;
        CreateMessageResultSample   content;
        OptMeta         /*vera*/    _meta;
    };
    using VecSamplingMessage        = std::vector<SamplingMessage>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#toolchoice
    enum class Mode {none, required, Auto}; // Actual "auto"
    using OptMode                   = std::optional<Mode>;
    struct ToolChoice
    {
        OptMode                     mode;
    };
    using OptToolChoice             = std::optional<ToolChoice>;

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#createmessagerequestparams
    enum class Where {none, thisServer, allServers};
    using OptWhere                  = std::optional<Where>;

    struct CreateMessageRequestParams
    {
        OptTaskMetadata             task;
        OptMeta         /*vera*/    _meta;
        VecSamplingMessage          messages;
        OptModelPreferences         modelPreferences;
        OptString                   systemPrompt;
        OptWhere                    includeContext;
        OptNumber                   temperature;
        Number                      maxTokens;
        OptVecString                stopSequences;
        OptAny                      metadata;
        OptVecTool                  tools;
        OptToolChoice               toolChoice;
    };

    // https://modelcontextprotocol.io/specification/2025-11-25/schema#createmessagerequest
    struct CreateMessageRequest
    {
        std::string                 jsonrpc; // “2.0”;
        RequestId                   id;
        std::string                 method; // “sampling/createMessage”;
        CreateMessageRequestParams  params;
    };

}

#endif
