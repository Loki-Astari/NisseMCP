#include "Local.h"
#include "JsonRPCCore.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
LocalContext::LocalContext(std::istream& input, std::ostream& output)
    : Context{input}
    , output{output}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
LocalContext::~LocalContext()
{
    // Close the output array.
    if (stream && count > 0) {
        output << "]";
    }
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

NISSEMCP_HEADER_ONLY_INCLUDE
std::ostream& LocalContext::addItem()
{
    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
    return output << sep;
}

NISSEMCP_HEADER_ONLY_INCLUDE
Local::~Local()
{}

NISSEMCP_HEADER_ONLY_INCLUDE
void Local::run(std::istream& input, std::ostream& output)
{
    while (true)
    {
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
        LocalContext    context(input, output);
        if (!getCore().handleInputStream(context)) {
            break;
        }
    }
}
