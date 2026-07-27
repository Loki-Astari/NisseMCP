#include "Context.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

Context::Context(std::istream& input)
    : input(input)
    , count{0}
    , stream(false)
{}

Context::~Context()
{}

void Context::serverSideStream()
{
    stream = true;
}

void Context::error(JsonRPC::OptRequestId id, int code, std::string_view message)
{
    addItem() << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, id}, outputConfig);
    ++count;
}

void Context::addNote()
{}
