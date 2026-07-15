#include "Context.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};
JsonRPC::OptRequestId                   Context::defaultId;

Context::Context(std::istream& input, std::ostream& output, Protocol protocol)
    : protocol(protocol)
    , input(input)
    , output(output)
    , requestId(defaultId)
    , stream(false)
{}

Context::~Context()
{}

void Context::serverSideStream()
{
    stream = true;
}

void Context::setId(JsonRPC::OptRequestId const& id)
{
    requestId = id;
}
