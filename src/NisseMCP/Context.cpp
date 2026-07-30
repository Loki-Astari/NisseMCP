#include "Context.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

NISSEMCP_HEADER_ONLY_INCLUDE
Context::Context(Session& session, std::istream& input)
    : session{session}
    , input{input}
    , count{0}
    , stream{false}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
Context::~Context()
{}

NISSEMCP_HEADER_ONLY_INCLUDE
void Context::serverSideStream()
{
    stream = true;
}

NISSEMCP_HEADER_ONLY_INCLUDE
void Context::error(JsonRPC::OptRequestId id, int code, std::string_view message)
{
    addItem() << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, id}, outputConfig);
    ++count;
}

NISSEMCP_HEADER_ONLY_INCLUDE
void Context::addNote()
{}
