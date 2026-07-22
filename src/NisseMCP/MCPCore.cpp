#include "MCPCore.h"

using namespace ThorsAnvil::Nisse::MCP;

MCPCore::MCPCore(Protocol protocol)
    : protocol{protocol}
{
    addExecutor("initialize",                [&](Context& context, Command::InitializeRequestParams const& param){return initialize(context, param);});
    addExecutor("notifications/initialized", [&](Context& context){return notifications_Initialized(context);});
}

void MCPCore::initialize(Context& context, Command::InitializeRequestParams const& param)
{
    using namespace std::string_literals;
    if (param.protocolVersion != "2025-11-25"s) {
        context.addItem(Command::InitializeResult{_meta: {}, protocolVersion: "2025-11-25"s, capabilities: {logging: {}, completions: {}, prompts: {}, resources: {}, tools: {}, tasks: {}}, serverInfo: {}, instructions: {}});
    }
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
