#include "Server.h"

using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
Server::Server(std::string_view slot, std::size_t workerCount, ThorsAnvil::ThorsSocket::ServerInit&& handlerInit, ThorsAnvil::ThorsSocket::ServerInit&& controlInit)
    : ThorsAnvil::Nisse::HTTP::Server{workerCount, std::forward<ThorsAnvil::ThorsSocket::ServerInit>(handlerInit), std::forward<ThorsAnvil::ThorsSocket::ServerInit>(controlInit)}
{
    addPath(ThorsAnvil::Nisse::HTTP::Method::POST, std::string{slot}, [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
    {
        handleRequest(request, response);
        return true;
    });
}

NISSEMCP_HEADER_ONLY_INCLUDE
Server::~Server()
{}
