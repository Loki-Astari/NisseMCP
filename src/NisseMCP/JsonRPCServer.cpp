#include "JsonRPCServer.h"
#include "ServerContext.h"

using namespace ThorsAnvil::Nisse::MCP;

JsonRPCCore& JsonRPCServer::getCore()
{
    return core;
}

void JsonRPCServer::handleRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    ServerContext     context{session, request, response};
    core.handleInputStream(context);
}
