#include "Local.h"
#include <tuple>

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    while (true)
    {
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
        LocalContext    context(input, output, getConfig().minProtocol);
        if (!context.handleInputStream(*this)) {
            break;
        }
    }
}
