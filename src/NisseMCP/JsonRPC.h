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

    using Id = std::variant<std::string, long>;
    struct IdWriter
    {
        ThorsAnvil::Serialize::Serializer&          parent;
        ThorsAnvil::Serialize::PrinterInterface&    printer;
        template<typename T>
        void operator()(T const& val) const
        {
            using Traits = ThorsAnvil::Serialize::Traits<T>;
            ThorsAnvil::Serialize::SerializerForBlock<Traits::type, T> serializer(parent, printer, std::any_cast<T>(val));
            serializer.printMembers();
        }
    };
    struct IdSizer
    {
        ThorsAnvil::Serialize::PrinterInterface&    printer;
        template<typename T>
        std::size_t operator()(T const& val) const
        {
            using Traits = ThorsAnvil::Serialize::Traits<T>;
            return Traits::getPrintSize(printer, val, true);
        }
    };
    class IdSerializer
    {
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
                    case ThorsAnvil::Serialize::ValueType::Number:   {long value;        parser.getValue(value);object = value;}
                    case ThorsAnvil::Serialize::ValueType::String:   {std::string value; parser.getValue(value);object = value;}
                    default:
                        ThorsLogAndThrowDebug(std::runtime_error, "ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer", "readCustom", "Expected integer or string for id");
                }
            }
    };
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
                : value(std::move(v))
                , outputer([](ThorsAnvil::Serialize::Serializer& parent, ThorsAnvil::Serialize::PrinterInterface& printer, std::any const& val)
                  {
                        using Traits = ThorsAnvil::Serialize::Traits<T>;
                        ThorsAnvil::Serialize::SerializerForBlock<Traits::type, T> serializer(parent, printer, std::any_cast<T>(val));
                        serializer.printMembers();
                  })
                , sizer([](ThorsAnvil::Serialize::PrinterInterface& printer, std::any const& val)
                  {
                        using Traits = ThorsAnvil::Serialize::Traits<T>;
                        return Traits::getPrintSize(printer, std::any_cast<T>(val), true);
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
                , result{std::move(result)}
            {}
            Response(int code, std::string&& message)
                : jsonrpc{"2.0"}
                , error{Error{code, std::move(message), {}}}
            {}
    };
}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::Id, ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::Result, ThorsAnvil::Nisse::MCP::JsonRPC::ResultSerializer);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Request, jsonrpc, method, params, id);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Error, code, message, data);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::JsonRPC::Response, jsonrpc, result, error, id);

#endif
