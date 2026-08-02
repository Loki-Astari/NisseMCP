#include "LocalContext.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
LocalContext::LocalContext(Session& session, std::istream& input, std::ostream& output)
    : Context{session, input}
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
