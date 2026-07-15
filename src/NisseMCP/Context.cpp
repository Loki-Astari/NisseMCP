#include "Context.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};
JsonRPC::OptRequestId                   Context::defaultId;

Context::Context(std::istream& input, std::ostream& output, Protocol protocol)
    : protocol(protocol)
    , input(input)
    , output(output)
    , requestId(defaultId)
    , errorState(false)
    , count(0)
    , stream(false)
{}

Context::~Context()
{
    // Close the output array.
    if (stream && count > 0) {
        output << "]";
    }
    if (errorState) {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void Context::stop()
{
    input.setstate(std::ios_base::failbit);
}

bool Context::good() const
{
    return input.good();
}

void Context::error(int code, std::string_view message, JsonRPC::OptRequestId const& id)
{
    errorState = true;
    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
    ++count;
    output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, id}, outputConfig);
}

void Context::setId(IdRef id)
{
    requestId = id;
}

void Context::serverSideStream()
{
    stream = true;
}
