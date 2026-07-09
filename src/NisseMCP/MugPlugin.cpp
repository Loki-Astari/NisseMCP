#include "MugPlugin.h"

#include "NisseHTTP/Request.h"

using namespace ThorsAnvil::Nisse::MCP;

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

void MugPlugin::processesRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    server.processesStream(request.body(), response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked));
}
