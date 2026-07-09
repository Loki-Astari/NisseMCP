#include "MugPlugin.h"
#include "JsonRPC.h"

#include "NisseHTTP/Request.h"

using namespace ThorsAnvil::NisseMCP;

MugPlugin::MugPlugin(MugPluginConfig const& configIn)
    : config{configIn}
{
    config.init = true;
}
ThorsAnvil::Nisse::HTTP::HTTPAction     action;

std::vector<ThorsAnvil::ThorsMug::Action> MugPlugin::getAction()
{
    static std::vector<ThorsAnvil::ThorsMug::Action> result
    {
        {
            ThorsAnvil::Nisse::HTTP::Method::GET,
            "/mcp",
            [](ThorsAnvil::Nisse::HTTP::Request const& requestStream, ThorsAnvil::Nisse::HTTP::Response& /*response*/)
            {
                JsonRPC     request{requestStream.body()};
                return true;
            }
        },
        {
            ThorsAnvil::Nisse::HTTP::Method::POST,
            "/mcp",
            [](ThorsAnvil::Nisse::HTTP::Request const& requestStream, ThorsAnvil::Nisse::HTTP::Response& /*response*/)
            {
                JsonRPC     request{requestStream.body()};
                return true;
            }
        }
    };

    return result;
}

void MugPlugin::resource()
{
}

void MugPlugin::tool()
{
}

void MugPlugin::prompt()
{
}
