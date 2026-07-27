#include "Context.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

Context::Context(std::istream& input)
    : input(input)
    , stream(false)
{}

Context::~Context()
{}

void Context::serverSideStream()
{
    stream = true;
}
