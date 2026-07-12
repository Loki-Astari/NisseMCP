#include "Server.h"
#include "JsonRPC.h"
#include "ThorSerialize/ParserConfig.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Server::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

Server::Server(ServerConfig const& /*config*/)
{
    addExecutor<SetLevelRequestParams>("logging/setLevel", [&](SetLevelRequestParams const& level) -> JsonRPC::Response {return loggingSetLevel(level);});
}

Server::State Server::processesStream(std::istream& input, std::ostream& output)
{
    using namespace std::string_view_literals;

    // Peek at first character to see if this is Batch or a single command.
    char nextChar;

    if (!(input >> nextChar)) {
        // No input.
        // This is probably because this is being called on stream in a loop.
        return Server::State::ErrorNoInput;
    }

    Server::State result = Server::State::OK;
    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        if (!(input >> nextChar)) {
            // If input fails then this is a parser error.
            output << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32700, "Parse error"}, outputConfig);
            return Server::State::ErrorReported;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            output << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32600, "Invalid Request"}, outputConfig);
            return Server::State::ErrorReported;
        }
        // Put back the next char we just stole for empty array checks.
        input.unget();

        std::size_t count = 0;
        while (nextChar != ']')
        {
            if (!processFunctionCall(input, output, count, (count == 0) ? "["sv : ","sv)) {
                // Bad Json. So we are going to exit.
                //           Other types of error allow us to continue.
                result = Server::State::ErrorReported;
                break;
            }
            if (!(input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                output << "," << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32700, "Parse error"}, outputConfig);
                result = Server::State::ErrorReported;
                break;
            }
        }
        // Close the output array.
        if (count > 0) {
            output << "]";
        }
    }
    else {
        // Put back the character we stole doing the check for an array.
        input.unget();
        // Scan like normal handling any potential issues.
        std::size_t count = 0;
        if (!processFunctionCall(input, output, count, ""sv)) {
            result = Server::State::ErrorReported;
        }
    }
    return result;
}

JsonRPC::Response Server::loggingSetLevel(SetLevelRequestParams const& /*level*/)
{
    // TODO
    return JsonRPC::Response{std::string{"OK"}};
}

bool Server::processFunctionCall(std::istream& input, std::ostream& output, std::size_t& count, std::string_view sep)
{
    JsonRPC::Request    rpc;
    if (!(input >> ThorsAnvil::Serialize::jsonImporter(rpc))) {
        output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32700, "Parse error"}, outputConfig);
        ++count;
        return false;
    }
    if (rpc.jsonrpc != "2.0") {
        output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32600, "Invalid Request", rpc.id}, outputConfig);
        ++count;
        return true;
    }

    auto find = executeMap.find(rpc.method);
    if (find == std::end(executeMap)) {
        output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{-32601, "Method not found", rpc.id}, outputConfig);
        ++count;
        return true;
    }

    JsonRPC::Response  result = (find->second)(rpc.params->getView());
    if (rpc.id.has_value()) {
        result.id   = rpc.id.value();
        ++count;
        output << sep << ThorsAnvil::Serialize::jsonExporter(result, outputConfig);
    }
    return true;
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
