#include "MugPlugin.h"
#include "JsonRPC.h"

#include "NisseHTTP/Request.h"
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::NisseMCP;

ThorsAnvil::Serialize::PrinterConfig    MugPlugin::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

MugPlugin::MugPlugin(MugServerConfig const& config)
    : server{config}
{}


std::vector<ThorsAnvil::ThorsMug::Action> MugPlugin::getAction()
{
    static std::vector<ThorsAnvil::ThorsMug::Action> result
    {
        {
            ThorsAnvil::Nisse::HTTP::Method::GET,
            "/mcp",
            [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
            {
                processesRequest(request, response);
                return true;
            }
        },
        {
            ThorsAnvil::Nisse::HTTP::Method::POST,
            "/mcp",
            [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
            {
                processesRequest(request, response);
                return true;
            }
        }
    };

    return result;
}

char MugPlugin::peekFirstNonWSChar(ThorsAnvil::Nisse::HTTP::Request const& request)
{
    char next = getFirstNonWSChar(request);
    request.body().unget();
    return next;
}

char MugPlugin::getFirstNonWSChar(ThorsAnvil::Nisse::HTTP::Request const& request)
{
    char next;
    request.body() >> next;
    return next;
}

void MugPlugin::processesRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    char firstChar = peekFirstNonWSChar(request);
    if (firstChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        char buff = getFirstNonWSChar(request);
        while (buff != ']') {
            processFunctionCall(request, response);
            buff = getFirstNonWSChar(request);
            if (buff != ',' && buff != ']') {
                // Error
            }
        }
    }
    else {
        processFunctionCall(request, response);
    }
}


void MugPlugin::processFunctionCall(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    JsonRPC::Request    rpc{request.body()};
    JsonRPC::Response   result = server.execute(rpc);
    if (rpc.id.has_value()) {
        std::size_t         size = ThorsAnvil::Serialize::jsonStreanSize(result);
        response.body(size) << ThorsAnvil::Serialize::jsonExporter(result, outputConfig);
    }
}
