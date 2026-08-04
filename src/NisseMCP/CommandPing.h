#ifndef THORSANVIL_NISSE_MCP_COMMAND_PING_H
#define THORSANVIL_NISSE_MCP_COMMAND_PING_H

#include "NisseMCPConfig.h"
#include "CommandCommon.h"
#include <string>

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
    OptObject                       result;
    JsonRPC::OptError               error;
};

/*
 * Used by the ClientMCP to understand what to send
 */
struct Ping
{
    using Request   = PingRequest;
    using Result    = PingResponse;
    using UserData  = Object;
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::PingRequest,         jsonrpc, id, method);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::Command::PingResponse,        jsonrpc, id, result, error);

#endif
