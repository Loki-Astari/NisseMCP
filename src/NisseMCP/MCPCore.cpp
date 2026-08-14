#include "MCPCore.h"
#include "MCPServerContext.h"
#include "JsonRPC.h"
#include "MCPRequestPing.h"
#include "MCPRequestLogging.h"
#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"
#include "ThorsLogging/ThorsLogging.h"
#include "ThorsLogging/loguru.hpp"


using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
MCPCore::MCPCore()
{
    addExecutor("initialize",                [&](Context& context, OptSerializableId id, JsonRPC::InitializeRequestParams const& param) {initialize(context, id, param);});
    addExecutor("notifications/initialized", [&](Context& context, OptSerializableId /*id*/)                                            {notifications_Initialized(context);});

    addExecutor("ping",                      [&](Context& context, OptSerializableId id)                                                {ping(context, id);});
    addExecutor("logging/setLevel",          [&](Context& context, OptSerializableId id, JsonRPC::SetLevelRequestParam const& param)    {loggingSetLevel(context, id, param);});
}

NISSEMCP_HEADER_ONLY_INCLUDE
bool MCPCore::supportBatchRequest(Context& context) const
{
    return context.session.supportBatchRequest();
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPCore::initialize(Context& context, OptSerializableId id, JsonRPC::InitializeRequestParams const& param)
{
    ThorsLogDebug("ThorsAnvil::Nisse::MCP::MCPCore", "initialize", "MCP Core Functionaliy");
    ProtocolRange protocolInfo = context.session.protocolRange();

    Protocol  defaultProtocol = param.protocolVersion;
    if (defaultProtocol < protocolInfo.first) {
        defaultProtocol =  protocolInfo.first;
    }
    else if (defaultProtocol > protocolInfo.second) {
        defaultProtocol = protocolInfo.second;
    }
    using namespace std::string_literals;
    context.addItem(id, JsonRPC::InitializeResult{
                                                // TODO ._meta          = {},
                                                .protocolVersion= defaultProtocol,
                                                .capabilities =
                                                {
                                                        .logging        = {},
                                                        .completions    = {},
                                                        .prompts        = {},
                                                        .resources      = {},
                                                        .tools          = {},
                                                        .tasks          = {}
                                                },
                                                .serverInfo     = {.name = std::string{context.session.serviceName()}},
                                                .instructions   = {}
                                                 });
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPCore::notifications_Initialized(Context& context)
{
    ThorsLogNotice("ThorsAnvil::Nisse::MCP::MCPCore", "notifications_Initialized", "MCP Core Functionaliy");
    MCPServerContext&   mcpContext  = dynamic_cast<MCPServerContext&>(context);
    auto const&         headers     = mcpContext.request.headers();
    auto const&         protocols   = headers.getHeader("MCP-Protocol-Version");
    if (protocols.size() != 1) {
        return;
    }
    std::string const&  protocolStr = protocols[0];
    Protocol            protocol    = ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::getValue(protocolStr, "");

    MCPSession&         mcpSession  = dynamic_cast<MCPSession&>(mcpContext.session);
    mcpSession.initialize(protocol);
}

void MCPCore::ping(Context& context, OptSerializableId id)
{
    ThorsLogNotice("ThorsAnvil::Nisse::MCP::MCPCore", "ping", "MCP Core Functionaliy");
    context.addItem(id, JsonRPC::Object{});
}

void MCPCore::loggingSetLevel(Context& context, OptSerializableId id, JsonRPC::SetLevelRequestParam const& param)
{
    ThorsLogNotice("ThorsAnvil::Nisse::MCP::MCPCore", "loggingSetLevel", "MCP Core Functionaliy");

#if 0
    Loguru Log Levels
    // Verbosity_FATAL   = -3,
    // Verbosity_ERROR   = -2,
    // Verbosity_WARNING = -1,
    // Verbosity_INFO    =  0,              // Default Log Log

    ThorsLogging
    LogFatal                => FATAL        Emergency
    LogError                => ERROR        Alert
    LogWarning              => WARNING      Critical
    LogInfo                 => INFO         Error
    LogDebug                => 3            Warning
    LogNote                 => 4            Notice
    LogTrack                => 5            Info
    LogTrace                => 7            Debug
    LogLowLevel             => 8            Trace
    LogFine                 => 9            // This is used only be things that log multiple times a second. Don't use it.

    MCP Logging Levels.
    emergency
    alert
    critical
    error                   LogError
    warning                 LogWarn
    notice
    info
    debug
    ----
#endif
    MCPSession&         mcpSession  = dynamic_cast<MCPSession&>(context.session);
    mcpSession.setLogging(param.level);

    context.addItem(id, JsonRPC::Object{});
}

#if 0
NISSEMCP_HEADER_ONLY_INCLUDE
void MCPCore::resource()
{
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPCore::tool()
{
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPCore::prompt()
{
}
#endif
