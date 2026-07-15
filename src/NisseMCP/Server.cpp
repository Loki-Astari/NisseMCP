#include "Server.h"
#include "JsonRPC.h"
#include "ThorSerialize/ParserConfig.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>

using namespace ThorsAnvil::Nisse::MCP;

Server::Server(ServerConfig const& /*config*/)
{}

bool Server::readOneAction(std::istream& input, Context& context)
{
    JsonRPC::Request    rpc;
    if (!(input >> ThorsAnvil::Serialize::jsonImporter(rpc))) {
        context.error(-32700, "Parse error", {});
        return false;
    }
    if (rpc.jsonrpc != "2.0") {
        context.error(-32600, "Invalid Request", rpc.id);
        return true;
    }

    auto find = executeMap.find(rpc.method);
    if (find == std::end(executeMap)) {
        context.error(-32601, "Method not found", rpc.id);
        return true;
    }

    (find->second)(context, rpc);
    return input.good();
}

#if 0
void Server::resource()
{
}

void Server::tool()
{
}

void Server::prompt()
{
}
#endif
