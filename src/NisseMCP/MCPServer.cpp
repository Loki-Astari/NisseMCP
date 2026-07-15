#include "MCPServer.h"

using namespace ThorsAnvil::Nisse::MCP;

Command::InitializeResult MCPServer::initialize(Command::InitializeRequestParams const& param)
{
    using namespace std::string_literals;
    if (param.protocolVersion != "2025-11-25"s) {
        return Command::InitializeResult{_meta: {}, protocolVersion: "2025-11-25"s, capabilities: {logging: {}, completions: {}, prompts: {}, resources: {}, tools: {}, tasks: {}}, serverInfo: {}, instructions: {}};
    }

    return {};
}

void MCPServer::notifications_Initialized()
{
}
