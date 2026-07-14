#ifndef THORSANVIL_NISSE_MCP_JSON_RCP_H
#define THORSANVIL_NISSE_MCP_JSON_RCP_H

#include "NisseMCPConfig.h"
#include <ThorsLogging/ThorsLogging.h>

#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>

#include <any>
#include <istream>
// https://www.jsonrpc.org/specification

namespace ThorsAnvil::Nisse::MCP::JsonRPC
{
    using Params = ThorsAnvil::Serialize::AnyBlock;
    using OptParams = std::optional<Params>;

    using Id = std::variant<std::string, long, char*>;
    class IdSerializer
    {
        struct IdWriter
        {
            ThorsAnvil::Serialize::Serializer&          parent;
            ThorsAnvil::Serialize::PrinterInterface&    printer;
            template<typename T>
            void operator()(T const& val) const
            {
                using Base = std::remove_cvref_t<T>;
                using Traits = ThorsAnvil::Serialize::Traits<Base>;
                Base const& val1 =  std::any_cast<Base>(val);
                ThorsAnvil::Serialize::SerializerForBlock<Traits::type, Base> serializer(parent, printer, val1);
                serializer.printMembers();
            }
        };
        struct IdSizer
        {
            ThorsAnvil::Serialize::PrinterInterface&    printer;
            template<typename T>
            std::size_t operator()(T const& val) const
            {
                using Base = std::remove_cvref_t<T>;
                using Traits = ThorsAnvil::Serialize::Traits<Base>;
                return Traits::getPrintSize(printer, val, true);
            }
        };
        public:
            static std::size_t getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, Id const& object)
            {
                return std::visit(IdSizer{printer}, object);
            }
            static void writeCustom(ThorsAnvil::Serialize::Serializer& parent, ThorsAnvil::Serialize::PrinterInterface& printer, Id const& object)
            {
                std::visit(IdWriter{parent, printer}, object);
            }
            static void readCustom(ThorsAnvil::Serialize::DeSerializer& /*parent*/, ThorsAnvil::Serialize::ParserInterface& parser, Id& object)
            {
                auto token = parser.getNextToken();
                if (token != ThorsAnvil::Serialize::ParserToken::Value) {
                    ThorsLogAndThrowDebug(std::runtime_error, "ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer", "readCustom", "Expected Value!");
                }
                switch (parser.peekType())
                {
                    case ThorsAnvil::Serialize::ValueType::Number:   {long value;        parser.getValue(value);object = value;break;}
                    case ThorsAnvil::Serialize::ValueType::String:   {std::string value; parser.getValue(value);object = value;break;}
                    default:
                        ThorsLogAndThrowDebug(std::runtime_error, "ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer", "readCustom", "Expected integer or string for id");
                }
            }
    };
    using OptId = std::optional<Id>;

    struct Request
    {
        std::string         jsonrpc;        // A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        std::string         method;         // A String containing the name of the method to be invoked.
        OptParams           params;
        OptId               id;
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

    struct Response
    {
        std::string         jsonrpc = "0.0";// A String specifying the version of the JSON-RPC protocol. MUST be exactly "2.0".
        OptResult           result;         // REQUIRED on success. MUST NOT exist if there was an error invoking the method.
        OptError            error;          // REQUIRED on error. MUST NOT exist if there was no error triggered during invocation.
        Id                  id;

        public:
            template<typename T>
            Response(T&& result)
                : jsonrpc{"2.0"}
                , result{std::forward<T>(result)}
                , id{static_cast<char*>(nullptr)}
            {}
            Response(int code, std::string&& message, OptId const& requestId)
                : jsonrpc{"2.0"}
                , error{Error{code, std::move(message), {}}}
                , id{static_cast<char*>(nullptr)}
            {
                if (requestId.has_value()) {
                    id = requestId.value();
                }
            }
            Response(int code, std::string&& message)
                : Response(code, std::forward<std::string>(message), {})
            {}
    };
}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::Id, ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::Result, ThorsAnvil::Nisse::MCP::JsonRPC::ResultSerializer);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Request, jsonrpc, method, params, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Error, code, message, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Response, jsonrpc, result, error, id);

#endif
