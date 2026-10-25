#include "MCPServerContext.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
ThorsAnvil::Nisse::HTTP::Response& MCPServerContext::addHeaders(ThorsAnvil::Nisse::HTTP::Response& response)
{
    return ServerContext::addHeaders(response).addHeader("MCP-Session-Id", dynamic_cast<MCPSession&>(session).toString());
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPServerContext::notifyThreadJoinContext(std::thread::id id)
{
    dynamic_cast<MCPSession&>(session).requestAdd(id);
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPServerContext::notifyThreadYieldContext(std::thread::id id)
{
    dynamic_cast<MCPSession&>(session).requestRemove(id);
}
