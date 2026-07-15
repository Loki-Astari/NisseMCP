#include "Local.h"
#include <tuple>

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    // ErrorNoInput means we hit the end of stream nothing was there.
    State state = State::OK;
    while (state != State::ErrorNoInput) {
        LocalContext    context(output);
        state = processesStream(input, context);
        if (state == State::ErrorNoInput) {
            break;
        }
        if (state == State::ErrorReported) {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
    }
}
