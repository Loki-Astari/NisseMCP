#include "JsonRPC.h"

using namespace ThorsAnvil::Nisse::MCP::JsonRPC;

NISSEMCP_HEADER_ONLY_INCLUDE
std::size_t ResultSerializer::getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, Result const& object)
{
    return object.sizer(printer, object.value);
}

NISSEMCP_HEADER_ONLY_INCLUDE
void ResultSerializer::writeCustom(ThorsAnvil::Serialize::Serializer& serializer, ThorsAnvil::Serialize::PrinterInterface& printer, Result const& object)
{
    object.outputer(serializer, printer, object.value);
}

NISSEMCP_HEADER_ONLY_INCLUDE
void ResultSerializer::readCustom(ThorsAnvil::Serialize::DeSerializer&, ThorsAnvil::Serialize::ParserInterface&, Result&)
{
    throw std::runtime_error("Deliberately not implemented");
}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientResponse::ClientResponse(void)
    : jsonrpc{"2.0"}
    , id{static_cast<char*>(nullptr)}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientResponse::ClientResponse(OptRequestId const& requestId)
    : jsonrpc{"2.0"}
    , id{static_cast<char*>(nullptr)}
{
    if (requestId.has_value()) {
        id = makeId(requestId.value());
    }
}

NISSEMCP_HEADER_ONLY_INCLUDE
ClientResponse::ClientResponse(int code, std::string_view message, OptRequestId const& requestId)
    : jsonrpc{"2.0"}
    , error{Error{code, {std::begin(message), std::end(message)}, {}}}
    , id{static_cast<char*>(nullptr)}
{
    if (requestId.has_value()) {
        id = makeId(requestId.value());
    }
}

NISSEMCP_HEADER_ONLY_INCLUDE
Response::Response(void)
    : jsonrpc{"2.0"}
    , result{}
    , id{static_cast<char*>(nullptr)}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
Response::Response(int code, std::string_view message, OptRequestId const& requestId)
    : jsonrpc{"2.0"}
    , error{Error{code, {std::begin(message), std::end(message)}, {}}}
    , id{static_cast<char*>(nullptr)}
{
    if (requestId.has_value()) {
        id = makeId(requestId.value());
    }
}
