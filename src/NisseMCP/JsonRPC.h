#ifndef THORSANVIL_NISSE_MCP_JSON_RCP_H
#define THORSANVIL_NISSE_MCP_JSON_RCP_H

#include "NisseMCPConfig.h"
#include "SerializableId.h"

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>

#include <any>
#include <string>
#include <string_view>
#include <istream>
#include <optional>
#include <variant>
#include <functional>
#include <utility>
#include <type_traits>

// https://www.jsonrpc.org/specification

namespace ThorsAnvil::Nisse::MCP::JsonRPC
{
    using Params = ThorsAnvil::Serialize::AnyBlock;
    using OptParams = std::optional<Params>;

    using RequestId  = SerializableId;
    using OptRequestId = std::optional<RequestId>;

    using ResponseId = SerializableNullId;
    using OptResponseId = std::optional<ResponseId>;

    struct ResponseConverter
    {
        template<typename T>
        ResponseId operator()(T const& val)   const {return val;}
    };
    inline ResponseId makeId(RequestId const& r)
    {
        return std::visit(ResponseConverter{}, r);
    }

    struct Request
    {
        std::string         jsonrpc;        // A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        std::string         method;         // A String containing the name of the method to be invoked.
        OptParams           params;
        OptRequestId        id;
    };

    class ResultSerializer;
    class Result
    {
        std::any                                                                                                                value;

        friend class ResultSerializer;
        std::function<void(ThorsAnvil::Serialize::Serializer&, ThorsAnvil::Serialize::PrinterInterface&, std::any const&)>      outputer;
        std::function<std::size_t(ThorsAnvil::Serialize::PrinterInterface&, std::any const&)>                                   sizer;
        public:
            template<typename T>
            Result(T&& v)
                : value(std::forward<T>(v))
                , outputer([](ThorsAnvil::Serialize::Serializer& parent, ThorsAnvil::Serialize::PrinterInterface& printer, std::any const& val)
                  {
                        using Base = std::decay_t<T>;
                        using Traits = ThorsAnvil::Serialize::Traits<Base>;
                        Base const& val1 = std::any_cast<Base>(val);
                        ThorsAnvil::Serialize::SerializerForBlock<Traits::type, Base> serializer(parent, printer, val1);
                        serializer.printMembers();
                  })
                , sizer([](ThorsAnvil::Serialize::PrinterInterface& printer, std::any const& val)
                  {
                        using Base = std::decay_t<T>;
                        using Traits = ThorsAnvil::Serialize::Traits<Base>;
                        Base const& val1 = std::any_cast<Base>(val);
                        return Traits::getPrintSize(printer, val1, true);
                  })
            {}
    };
    class ResultSerializer
    {
        public:
            static std::size_t getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, Result const& object)
            {
                return object.sizer(printer, object.value);
            }
            static void writeCustom(ThorsAnvil::Serialize::Serializer& serializer, ThorsAnvil::Serialize::PrinterInterface& printer, Result const& object)
            {
                object.outputer(serializer, printer, object.value);
            }
            static void readCustom(ThorsAnvil::Serialize::DeSerializer&, ThorsAnvil::Serialize::ParserInterface& parser, Result& object);
    };

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

    // We can not 'import' an std::any. So we must know the expected result on the client side.
    struct ClientResponse
    {
        std::string         jsonrpc = "0.0";// A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        // OptResult           result;         // REQUIRED on success. MUST NOT exist if there was an error invoking the method.
        OptError            error;          // REQUIRED on error. MUST NOT exist if there was no error triggered during invocation.
        OptResponseId       id;

        public:
            ClientResponse(void)
                : jsonrpc{"2.0"}
                , id{static_cast<char*>(nullptr)}
            {}
            ClientResponse(OptRequestId const& requestId)
                : jsonrpc{"2.0"}
                , id{static_cast<char*>(nullptr)}
            {
                if (requestId.has_value()) {
                    id = makeId(requestId.value());
                }
            }
            ClientResponse(int code, std::string_view message, OptRequestId const& requestId)
                : jsonrpc{"2.0"}
                , error{Error{code, {std::begin(message), std::end(message)}, {}}}
                , id{static_cast<char*>(nullptr)}
            {
                if (requestId.has_value()) {
                    id = makeId(requestId.value());
                }
            }
    };

    // We can build and serialize a normal response object on the server as we know the type as we generate the output.
    struct Response
    {
        std::string         jsonrpc = "0.0";// A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        OptResult           result;         // REQUIRED on success. MUST NOT exist if there was an error invoking the method.
        OptError            error;          // REQUIRED on error. MUST NOT exist if there was no error triggered during invocation.
        OptResponseId       id;

        public:
            Response(void)
                : jsonrpc{"2.0"}
                , result{}
                , id{static_cast<char*>(nullptr)}
            {}
            template<typename T>
            Response(T&& result, OptRequestId const& requestId)
                : jsonrpc{"2.0"}
                , result{std::forward<T>(result)}
                , id{static_cast<char*>(nullptr)}
            {
                if (requestId.has_value()) {
                    id = makeId(requestId.value());
                }
            }
            Response(int code, std::string_view message, OptRequestId const& requestId)
                : jsonrpc{"2.0"}
                , error{Error{code, {std::begin(message), std::end(message)}, {}}}
                , id{static_cast<char*>(nullptr)}
            {
                if (requestId.has_value()) {
                    id = makeId(requestId.value());
                }
            }
    };
}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::Result, ThorsAnvil::Nisse::MCP::JsonRPC::ResultSerializer);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Request, jsonrpc, method, params, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Error, code, message, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::ClientResponse, jsonrpc, error, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Response, jsonrpc, result, error, id);

#endif
