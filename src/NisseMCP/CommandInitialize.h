#ifndef THORSANVIL_NISSE_MCP_COMMAND_INITIALIZE_H
#define THORSANVIL_NISSE_MCP_COMMAND_INITIALIZE_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include "Context.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#initialize
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#clientcapabilities
// Serialized
struct Roots
{
    OptBool                 listChanged;
    OptBool                 subscribe;
};
using OptRoots              = std::optional<Roots>;
// Serialized
struct Sampling
{
    OptObject               context;
    OptObject               tools;
};
using OptSampling           = std::optional<Sampling>;
// Serialized
struct Elicitation
{
    OptObject               form;
    OptObject               url;
};
using OptElicitation        = std::optional<Elicitation>;
// Serialized
struct RequestSampling
{
    OptObject               createMessage;
};
using OptRequestSampling    = std::optional<Sampling>;
// Serialized
struct RequestElicitation
{
    OptObject               create;
};
using OptRequestElicitation = std::optional<Elicitation>;
// Serialized
struct Request
{
    OptRequestSampling      sampling;
    OptRequestElicitation   elicitation;
};
using OptRequest            = std::optional<Request>;
// Serialized
struct Tasks
{
    OptObject               list;
    OptObject               cancel;
    OptRequest              requests;
};
using OptTasks              = std::optional<Tasks>;
// Serialized
struct ClientCapabilities
{
    // experimental?: { [key: string]: object };
    OptRoots                roots;
    OptSampling             sampling;
    OptElicitation          elicitation;
    OptTasks                tasks;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#implementation
// Serialized
struct Implementation
{
    OptVecIcon              icons;
    std::string             name;
    OptString               title;
    OptString               version;
    OptString               description;
    OptString               websiteUrl;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#initializerequestparams
// Serialized
struct InitializeRequestParams
{
    OptMeta     /*vera*/    _meta;
    Protocol                protocolVersion;
    ClientCapabilities      capabilities;
    Implementation          clientInfo;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#servercapabilities
// Serialized
struct Caller
{
    OptObject               call;
};
using OptCaller             = std::optional<Caller>;
// Serialized
struct Tools
{
    Caller                  tools;
};
using OptTools              = std::optional<Tools>;
// Serialized
struct ServerTasks
{
    OptObject               list;
    OptObject               cancel;
    OptTools                requests;
};
using OptServerTasks        = std::optional<ServerTasks>;
// Serialized
struct ServerCapabilities
{
    // experimental?: { [key: string]: object };
    OptObject               logging;
    OptObject               completions;
    OptRoots                prompts;
    OptRoots                resources;
    OptRoots                tools;
    OptServerTasks          tasks;
};

// https://modelcontextprotocol.io/specification/2025-11-25/schema#initializeresult
// Serialized
struct InitializeResult
{
    OptMeta     /*vera*/    _meta;
    Protocol                protocolVersion;
    ServerCapabilities      capabilities;
    Implementation          serverInfo;
    OptString               instructions;
    // [key: string]: unknown;
};
using OptInitializeResult = std::optional<InitializeResult>;

// https://modelcontextprotocol.io/specification/2025-11-25/schema#initializerequest
struct InitializeRequest
{
    std::string             jsonrpc; // “2.0”;
    RequestId               id;
    std::string             method; //  “initialize”;
    InitializeRequestParams params;
    InitializeRequest(RequestId&& id, InitializeRequestParams&& params)
        : jsonrpc{"2.0"}
        , id{std::move(id)}
        , method{"initialize"}
        , params{std::move(params)}
    {}
};

// Used by Client.
// Client can't use JsonRPC::Response directly as the result field is a std::any
// So this result is specific in what we expect back from the server.
struct InitializeResponse
{
    std::string         jsonrpc = "0.0";// A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
    OptInitializeResult result;         // REQUIRED on success. MUST NOT exist if there was an error invoking the method.
    JsonRPC::OptError   error;          // REQUIRED on error. MUST NOT exist if there was no error triggered during invocation.
    JsonRPC::ResponseId id;
};


struct Notification_Initialized
{
    std::string         jsonrpc;
    std::string         method;
    Notification_Initialized()
        : jsonrpc{"2.0"}
        , method{"notifications/initialized"}
    {}
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Roots,                    listChanged, subscribe);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Sampling,                 context, tools);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Elicitation,              form, url);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::RequestSampling,          createMessage);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::RequestElicitation,       create);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Request,                  sampling, elicitation);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Tasks,                    list, cancel, requests);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::ClientCapabilities,       roots, sampling, elicitation, tasks);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Implementation,           icons, name, title, version, description, websiteUrl);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::InitializeRequestParams,  _meta, protocolVersion, capabilities, clientInfo);

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Caller,                   call);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Tools,                    tools);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::ServerTasks,              list, cancel, requests);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::ServerCapabilities,       logging, completions, prompts, resources, tools, tasks);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::InitializeResult,         _meta, protocolVersion, capabilities, serverInfo, instructions);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::InitializeRequest,        jsonrpc, id, method, params);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::Notification_Initialized, jsonrpc, method);

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::InitializeResponse,       jsonrpc, result, error, id);

#endif
