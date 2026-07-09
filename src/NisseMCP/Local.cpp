#include "Local.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(MugServerConfig const& config)
    : server{config}
{}

void Local::run()
{
    while (server.processesStream(std::cin, std::cout)) {
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
    }
}
