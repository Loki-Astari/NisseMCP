#include "MCPServer.h"

using namespace ThorsAnvil::Nisse::MCP;

void MCPServer::initialize(Context& context, Command::InitializeRequestParams const& param)
{
    using namespace std::string_literals;
    if (param.protocolVersion != "2025-11-25"s) {
        context.addItem(Command::InitializeResult{_meta: {}, protocolVersion: "2025-11-25"s, capabilities: {logging: {}, completions: {}, prompts: {}, resources: {}, tools: {}, tasks: {}}, serverInfo: {}, instructions: {}});
    }
}

void MCPServer::notifications_Initialized(Context& /*context*/)
{
}
