#include "MCPCore.h"
#include "MCPServerContext.h"
#include "JsonRPC.h"
#include "CommandPing.h"
#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"


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
    ThorsLogNote("ThorsAnvil::Nisse::MCP::MCPCore", "initialize", "MCP Core Functionaliy");
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
                                                ._meta          = {},
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
    ThorsLogNote("ThorsAnvil::Nisse::MCP::MCPCore", "notifications_Initialized", "MCP Core Functionaliy");
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
    ThorsLogNote("ThorsAnvil::Nisse::MCP::MCPCore", "ping", "MCP Core Functionaliy");
    context.addItem(id, Command::Object{});
}

void MCPCore::loggingSetLevel(Context& context, JsonRPC::OptRequestId id, Command::SetLevelRequestParams const& /*param*/)
{
    ThorsLogNote("ThorsAnvil::Nisse::MCP::MCPCore", "loggingSetLevel", "MCP Core Functionaliy");
    // TODO Needs real implementation.
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
