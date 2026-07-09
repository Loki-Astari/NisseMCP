#ifndef THORSANVIL_NISSE_MCP_JSON_RCP_H
#define THORSANVIL_NISSE_MCP_JSON_RCP_H

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <istream>

// https://www.jsonrpc.org/specification

namespace ThorsAnvil::Nisse::MCP::JsonRPC
{
    using Params = ThorsAnvil::Serialize::AnyBlock;
    using OptParams = std::optional<Params>;

    using Id = std::string;
    using OptId = std::optional<Id>;

    struct Request
    {
        Request(std::istream& stream);
        Request(std::string_view view);


        std::string         jsonrpc;        // A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        std::string         method;         // A String containing the name of the method to be invoked.
        OptParams           params;
        OptId               id;
    };


    using Result = std::string;
    using OptResult = std::optional<Result>;

    using Data = std::string;
    using OptData = std::optional<Data>;

    struct Error
    {
        int                 code;
        std::string         message;
        OptData             data;
    };
    using OptError = std::optional<Error>;

    struct Response
    {
        std::string         jsonrpc = "2.0";// A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        OptResult           result;         // REQUIRED on success. MUST NOT exist if there was an error invoking the method.
        OptError            error;          // REQUIRED on error. MUST NOT exist if there was no error triggered during invocation.
        Id                  id;
    };
}


ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Request, jsonrpc, method, params, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Error, code, message, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Response, jsonrpc, result, error, id);

#endif
