#include "JsonRPCServer.h"
#include "ServerContext.h"

using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
JsonRPCCore& JsonRPCServer::getCore()
{
    return core;
}

NISSEMCP_HEADER_ONLY_INCLUDE
void JsonRPCServer::handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    ServerContext     context{session, request, response};
    core.handleInputStream(context);
}
