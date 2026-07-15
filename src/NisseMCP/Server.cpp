#include "Server.h"
#include "JsonRPC.h"
#include "ThorSerialize/ParserConfig.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Server::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

Server::Server(ServerConfig const& /*config*/)
{}

State Server::processesStream(std::istream& input, Context& context)
{
    using namespace std::string_view_literals;

    // Peek at first character to see if this is Batch or a single command.
    char nextChar;

    if (!(input >> nextChar)) {
        // No input.
        // This is probably because this is being called on stream in a loop.
        context.error(State::ErrorNoInput);
        return State::ErrorNoInput;
    }

    State result = State::OK;
    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        if (!(input >> nextChar)) {
            // If input fails then this is a parser error.
            context.error(State::ErrorReported) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32700, "Parse error"}, outputConfig);
            return State::ErrorReported;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            context.error(State::ErrorReported) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32600, "Invalid Request"}, outputConfig);
            return State::ErrorReported;
        }
        // Put back the next char we just stole for empty array checks.
        input.unget();

        context.serverSideStream();

        while (nextChar != ']')
        {
            if (!processFunctionCall(input, context)) {
                // Bad Json. So we are going to exit.
                //           Other types of error allow us to continue.
                result = State::ErrorReported;
                break;
            }
            if (!(input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                context.error(State::ErrorReported) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32700, "Parse error"}, outputConfig);
                result = State::ErrorReported;
                break;
            }
        }
    }
    else {
        // Put back the character we stole doing the check for an array.
        input.unget();
        if (!processFunctionCall(input, context)) {
            result = State::ErrorReported;
        }
    }
    return result;
}

bool Server::processFunctionCall(std::istream& input, Context& context)
{
    JsonRPC::Request    rpc;
    if (!(input >> ThorsAnvil::Serialize::jsonImporter(rpc))) {
        context.error(State::ErrorReported) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32700, "Parse error"}, outputConfig);
        return false;
    }
    if (rpc.jsonrpc != "2.0") {
        context.error(State::ErrorItem) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32600, "Invalid Request", rpc.id}, outputConfig);
        return true;
    }

    auto find = executeMap.find(rpc.method);
    if (find == std::end(executeMap)) {
        context.error(State::ErrorItem) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32601, "Method not found", rpc.id}, outputConfig);
        return true;
    }

    using namespace std::string_view_literals;
    JsonRPC::Response  result = (find->second)(rpc.params.has_value() ? rpc.params->getView() : ""sv);
    if (rpc.id.has_value()) {
        result.id   = JsonRPC::makeId(rpc.id.value());
        context.addItem() << ThorsAnvil::Serialize::jsonExporter(result, outputConfig);
    }
    return true;
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
