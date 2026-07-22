#include "Local.h"

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;


LocalContext::LocalContext(std::istream& input, std::ostream& output, Protocol protocol)
    : Context(input, output, protocol)
    , count(0)
{}

LocalContext::~LocalContext()
{
    // Close the output array.
    if (stream && count > 0) {
        output << "]";
    }
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void LocalContext::stop() const
{
    input.setstate(std::ios_base::failbit);
}

std::ostream& LocalContext::addItem()
{
    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
    ++count;
    return output << sep;
}
