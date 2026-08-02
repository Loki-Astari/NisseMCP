#ifndef THORSANVIL_NISSE_MCP_COMMAND_PING_H
#define THORSANVIL_NISSE_MCP_COMMAND_PING_H

#include <string>
#include "NisseMCPConfig.h"
#include "CommandCommon.h"

// https://modelcontextprotocol.io/specification/2025-11-25/schema#ping
namespace ThorsAnvil::Nisse::MCP::Command
{

// https://modelcontextprotocol.io/specification/2025-11-25/schema#pingrequest
struct PingRequest
{
    std::string                     jsonrpc; // “2.0”;
    RequestId                       id;
    std::string                     method; // “ping”;
    PingRequest(RequestId&& id)
        : jsonrpc{"2.0"}
        , id{std::move(id)}
        , method{"ping"}
    {}
};

struct PingResponse
{
    std::string                     jsonrpc;
    RequestId                       id;
    Object                          result;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::PingRequest,         jsonrpc, id, method);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::PingResponse,        jsonrpc, id, result);

#endif
