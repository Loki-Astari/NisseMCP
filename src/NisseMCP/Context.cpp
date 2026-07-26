#include "Context.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};
JsonRPC::OptRequestId                   Context::defaultId;

Context::Context(std::istream& input)
    : input(input)
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
