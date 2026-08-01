#ifndef THORSANVIL_NISSE_MCP_SERIALIZABLE_ID
#define THORSANVIL_NISSE_MCP_SERIALIZABLE_ID

#include "NisseMCPConfig.h"

#include <ThorsLogging/ThorsLogging.h>
#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>

#include <variant>
#include <string>
#include <type_traits>
#include <any>
#include <stdexcept>

namespace ThorsAnvil::Nisse::MCP::JsonRPC
{

// An object that can be a String / Number / Null
using SerializableNullId            = std::variant<std::string, long, char*>;
using SerializableId                = std::variant<std::string, long>;

template<bool supportNull>
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
        template<typename T>
        static std::size_t getPrintSize(ThorsAnvil::Serialize::PrinterInterface& printer, T const& object)
        {
            return std::visit(IdSizer{printer}, object);
        }
        template<typename T>
        static void writeCustom(ThorsAnvil::Serialize::Serializer& parent, ThorsAnvil::Serialize::PrinterInterface& printer, T const& object)
        {
            std::visit(IdWriter{parent, printer}, object);
        }
        template<typename T>
        static void readCustom(ThorsAnvil::Serialize::DeSerializer& /*parent*/, ThorsAnvil::Serialize::ParserInterface& parser, T& object)
        {
            auto token = parser.getNextToken();
            if (token != ThorsAnvil::Serialize::ParserToken::Value) {
                ThorsLogAndThrowDebug(std::runtime_error, "ThorsAnvil::Nisse::MCP::IdSerializer", "readCustom", "Expected Value!");
            }
            switch (parser.peekType())
            {
                case ThorsAnvil::Serialize::ValueType::Number:  {long value;        parser.getValue(value);object = value;break;}
                case ThorsAnvil::Serialize::ValueType::String:  {std::string value; parser.getValue(value);object = value;break;}

                // The Null case will fall through to throw if 'supportNull' is false;
                // But if we support true than a null on the input results in a nullptr being added to the variant.
                case ThorsAnvil::Serialize::ValueType::Null:    {if constexpr (!supportNull) {std::string value; parser.getValue(value);object = static_cast<char*>(nullptr);break;}}

                default:
                    ThorsLogAndThrowDebug(std::runtime_error, "ThorsAnvil::Nisse::MCP::IdSerializer", "readCustom", "Expected integer or string for id");
            }
        }
};

}

ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::SerializableNullId,            ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer<true>);
ThorsAnvil_MakeTraitCustomSerialize(ThorsAnvil::Nisse::MCP::JsonRPC::SerializableId,                ThorsAnvil::Nisse::MCP::JsonRPC::IdSerializer<false>);

#endif
