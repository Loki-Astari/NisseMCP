#include "Local.h"
#include <tuple>

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    LocalContext    context(output);
    // ErrorNoInput means we hit the end of stream nothing was there.
    for (Server::State state = processesStream(input, context); state != Server::State::ErrorNoInput; state = processesStream(input, context)) {
        if (state == Server::State::ErrorReported) {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
    }
}
