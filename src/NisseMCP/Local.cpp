#include "Local.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    while (processesStream(input, output)) {
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
    }
}
