#ifndef THORSANVIL_NISSE_MCP_CONTEXT_H
#define THORSANVIL_NISSE_MCP_CONTEXT_H

#include "JsonRPC.h"
#include <ThorSerialize/JsonThor.h>

#include <iostream>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{
    enum class Protocol {v2024_11_05, v2025_03_26, v2025_06_18, v2025_11_25};

    using IdRef = std::reference_wrapper<const JsonRPC::OptRequestId>;
    class Context
    {
        static ThorsAnvil::Serialize::PrinterConfig    outputConfig;
        static JsonRPC::OptRequestId                   defaultId;

        Protocol        protocol;
        std::istream&   input;
        std::ostream&   output;
        IdRef           requestId;
        bool            errorState;
        std::size_t     count;
        bool            stream;
        public:
            Context(std::istream& input, std::ostream& output, Protocol protocol = Protocol::v2025_11_25)
                : protocol(protocol)
                , input(input)
                , output(output)
                , requestId(defaultId)
                , errorState(false)
                , count(0)
                , stream(false)
            {}
            ~Context()
            {
                // Close the output array.
                if (stream && count > 0) {
                    output << "]";
                }
                if (errorState) {
                    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
            void stop()
            {
                input.setstate(std::ios_base::failbit);
            }
            bool good() const
            {
                return input.good();
            }
            void error(int code, std::string_view message, JsonRPC::OptRequestId const& id)
            {
                errorState = true;
                std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
                ++count;
                output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, id}, outputConfig);
            }
            void setId(IdRef id)
            {
                requestId = id;
            }
            void serverSideStream()
            {
                stream = true;
            }
            template<typename T>
            void  addItem(T const& value)
            {
                if (requestId.get().has_value()) {
                    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
                    ++count;
                    output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{value, requestId}, outputConfig);
                }
            }
    };
}

#endif
