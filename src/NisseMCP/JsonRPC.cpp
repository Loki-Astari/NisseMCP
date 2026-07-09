#include "JsonRPC.h"
#include "ThorSerialize/JsonThor.h"

using namespace ThorsAnvil::Nisse::MCP::JsonRPC;

Request::Request(std::string_view view)
{
    view >> ThorsAnvil::Serialize::jsonImporter(*this);
}
Request::Request(std::istream& request)
{
    request >> ThorsAnvil::Serialize::jsonImporter(*this);
}
