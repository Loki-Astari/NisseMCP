#include "MCPCore.h"
#include "MCPServerContext.h"
#include "JsonRPC.h"
#include "CommandPing.h"
#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"
#include "ThorsLogging/ThorsLogging.h"
#include "ThorsLogging/loguru.hpp"


using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
MCPCore::MCPCore()
{
    addExecutor("initialize",                [&](Context& context, JsonRPC::OptRequestId id, Command::InitializeRequestParams const& param) {initialize(context, id, param);});
    addExecutor("notifications/initialized", [&](Context& context, JsonRPC::OptRequestId /*id*/)                                            {notifications_Initialized(context);});

    addExecutor("ping",                      [&](Context& context, JsonRPC::OptRequestId id)                                                {ping(context, id);});
    addExecutor("logging/setLevel",          [&](Context& context, JsonRPC::OptRequestId id, Command::SetLevelRequestParams const& param)   {loggingSetLevel(context, id, param);});
}

NISSEMCP_HEADER_ONLY_INCLUDE
bool MCPCore::supportBatchRequest(Context& context) const
{
    return context.session.supportBatchRequest();
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPCore::initialize(Context& context, JsonRPC::OptRequestId id, Command::InitializeRequestParams const& param)
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
    context.addItem(id, Command::InitializeResult{
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
    ThorsLogDebug("ThorsAnvil::Nisse::MCP::MCPCore", "notifications_Initialized", "MCP Core Functionaliy");
    MCPServerContext&   mcpContext  = dynamic_cast<MCPServerContext&>(context);
    MCPSession&         mcpSession  = dynamic_cast<MCPSession&>(mcpContext.session);
    auto const&         headers     = mcpContext.request.headers();
    auto const&         protocols   = headers.getHeader("MCP-Protocol-Version");
    if (protocols.size() != 1) {
        return;
    }
    std::string const&  protocolStr = protocols[0];
    Protocol            protocol    = ThorsAnvil::Serialize::Traits<ThorsAnvil::Nisse::MCP::Protocol>::getValue(protocolStr, "");
    mcpSession.initialize(protocol);
}

void MCPCore::ping(Context& context, JsonRPC::OptRequestId id)
{
    ThorsLogDebug("ThorsAnvil::Nisse::MCP::MCPCore", "ping", "MCP Core Functionaliy");
    context.addItem(id, Command::Object{});
}

void MCPCore::loggingSetLevel(Context& context, JsonRPC::OptRequestId id, Command::SetLevelRequestParams const& /*param*/)
{
    ThorsLogDebug("ThorsAnvil::Nisse::MCP::MCPCore", "loggingSetLevel", "MCP Core Functionaliy");

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
    loguru::Verbosity  verbosity;
    loguru::add_callback("'MCPLog'", mcpLog, nullptr, verbosity, syslog_close, syslog_flush);

    //
    context.addItem(id, Command::Object{});
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
