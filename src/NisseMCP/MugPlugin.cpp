#include "MugPlugin.h"
#include <sstream>

// Temporary: We will need our own context.
//            But use this as a stop gap.
#include "Local.h"

#include "NisseHTTP/Request.h"

using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
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

NISSEMCP_HEADER_ONLY_INCLUDE
void MugPlugin::processesRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
{
    // MIY TODO: Fix Context
    LocalContext        context(request.body(), response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked));
    // handleInputStream(context);
}
