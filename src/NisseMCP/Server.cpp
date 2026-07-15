#include "Server.h"
#include "JsonRPC.h"
#include "ThorSerialize/ParserConfig.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>

using namespace ThorsAnvil::Nisse::MCP;

Server::Server(ServerConfig const& /*config*/)
{}

void Server::processesStream(std::istream& input, Context& context)
{
    using namespace std::string_view_literals;

    // Peek at first character to see if this is Batch or a single command.
    char nextChar;

    if (!(input >> nextChar)) {
        // No input.
        // This is probably because this is being called on stream in a loop.
        return;
    }

    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        if (!(input >> nextChar)) {
            // If input fails then this is a parser error.
            context.error(-32700, "Parse error", {});
            context.stop();
            return;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            context.error(-32600, "Invalid Request", {});
            context.stop();
            return;
        }
        // Put back the next char we just stole for empty array checks.
        input.unget();

        context.serverSideStream();

        while (nextChar != ']')
        {
            if (!processFunctionCall(input, context)) {
                // Bad Json. So we are going to exit.
                //           Other types of error allow us to continue.
                return;
            }
            if (!(input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                context.error(-32700, "Parse error", {});
                context.stop();
                return;
            }
        }
    }
    else {
        // Put back the character we stole doing the check for an array.
        input.unget();
        processFunctionCall(input, context);
    }
    return;
}

bool Server::processFunctionCall(std::istream& input, Context& context)
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
    return context.good();
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
