#include "Local.h"
#include "LocalContext.h"
#include "JsonRPCCore.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
Local::~Local()
{}

NISSEMCP_HEADER_ONLY_INCLUDE
void Local::run(Session& session, std::istream& input, std::ostream& output)
{
    while (true)
    {
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
        LocalContext    context(session, input, output);
        if (!getCore().handleInputStream(context)) {
            break;
        }
    }
}
