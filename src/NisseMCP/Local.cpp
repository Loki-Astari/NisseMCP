#include "Local.h"
#include <tuple>

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    State state = State::OK;
    while (true) {
        LocalContext    context(output);
        processesStream(input, context);
        state = context.getState();
        if (!input.good()) {
            break;
        }
        if (state == State::ErrorReported) {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
    }
}
