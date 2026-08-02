#include "MCPServerContext.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
ThorsAnvil::Nisse::HTTP::Response& MCPServerContext::addHeaders(ThorsAnvil::Nisse::HTTP::Response& response)
{
    return ServerContext::addHeaders(response).addHeader("MCP-Session-Id", dynamic_cast<MCPSession&>(session).toString());
}
