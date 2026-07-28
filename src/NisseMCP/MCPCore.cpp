#include "MCPCore.h"
#include "JsonRPC.h"
#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"

using namespace ThorsAnvil::Nisse::MCP;

MCPCore::MCPCore(Protocol protocol)
    : protocol{protocol}
{
    addExecutor("initialize",                [&](Context& context, JsonRPC::OptRequestId id, Command::InitializeRequestParams const& param){return initialize(context, id, param);});
    addExecutor("notifications/initialized", [&](Context& context, JsonRPC::OptRequestId /*id*/){return notifications_Initialized(context);});
}

void MCPCore::initialize(Context& context, JsonRPC::OptRequestId id, Command::InitializeRequestParams const& /*param*/)
{
    using namespace std::string_literals;
    context.addItem(id, Command::InitializeResult{
                                                ._meta          = {},
                                                .protocolVersion= "2025-11-25"s,
                                                .capabilities =
                                                {
                                                        .logging        = {},
                                                        .completions    = {},
                                                        .prompts        = {},
                                                        .resources      = {},
                                                        .tools          = {},
                                                        .tasks          = {}
                                                },
                                                .serverInfo     = {},
                                                .instructions   = {}
                                                 });
}

void MCPCore::notifications_Initialized(Context& /*context*/)
{
}
#if 0
void MCPCore::resource()
{
}

void MCPCore::tool()
{
}

void MCPCore::prompt()
{
}
#endif
