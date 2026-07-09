#include "Server.h"

using namespace ThorsAnvil::NisseMCP;

Server::Server(MugServerConfig const& /*config*/)
{}

JsonRPC::Response Server::execute(JsonRPC::Request const& /*request*/)
{
    return {};
}

void Server::resource()
{
}

void Server::tool()
{
}

void Server::prompt()
{
}
