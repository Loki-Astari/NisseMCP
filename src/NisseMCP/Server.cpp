#include "Server.h"
#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>
#include "NisseMCP/JsonRPC.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Server::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

Server::Server(ServerConfig const& /*config*/)
{
    addExecutor<SetLevelRequestParams>("logging/setLevel", [&](SetLevelRequestParams const& level) -> JsonRPC::Response {return loggingSetLevel(level);});
}

bool Server::processesStream(std::istream& input, std::ostream& output)
{
    // Peek at first character to see if this is Batch or a single command.
    char nextChar;
    input >> nextChar;

    if (!input) {
        return false;
    }

    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        while (nextChar != ']') {
            processFunctionCall(input, output);
            input >> nextChar;
            if (nextChar != ',' && nextChar != ']') {
                // Error
            }
        }
    }
    else {
        // Put back the character we stole doing the check.
        // Scan like normal handling any potential issues.
        input.unget();
        processFunctionCall(input, output);
    }
    return true;
}

JsonRPC::Response Server::loggingSetLevel(SetLevelRequestParams const& /*level*/)
{
    // TODO
    return JsonRPC::Response{std::string{"OK"}};
}

void Server::processFunctionCall(std::istream& input, std::ostream& output)
{
    JsonRPC::Request    rpc{input};
    JsonRPC::Response   result = execute(rpc);
    if (rpc.id.has_value()) {
        result.id.assign(rpc.id.value());
        output << ThorsAnvil::Serialize::jsonExporter(result, outputConfig);
    }
}

JsonRPC::Response Server::execute(JsonRPC::Request const& request)
{
    auto find = executeMap.find(request.method);
    if (find == std::end(executeMap)) {
        return JsonRPC::Response{12, "No Func"};
    }

    JsonRPC::Response  result = (find->second)(request.params->getView());
    return result;
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
