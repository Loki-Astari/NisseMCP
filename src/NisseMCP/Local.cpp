#include "Local.h"
#include <tuple>

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    while (true) {
        LocalContext    context(input, output);
        context.handleInputStream(*this);

        if (!input.good()) {
            break;
        }
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
    }
}
