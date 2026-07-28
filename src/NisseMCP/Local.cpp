#include "Local.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;


LocalContext::LocalContext(std::istream& input, std::ostream& output)
    : Context{input}
    , output{output}
{}

LocalContext::~LocalContext()
{
    // Close the output array.
    if (stream && count > 0) {
        output << "]";
    }
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::ostream& LocalContext::addItem()
{
    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
    return output << sep;
}

Local::~Local()
{}

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
