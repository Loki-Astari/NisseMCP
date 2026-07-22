#ifndef THORSANVIL_NISSE_MCP_CONTEXT_H
#define THORSANVIL_NISSE_MCP_CONTEXT_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"
#include <ThorSerialize/JsonThor.h>

#include <iostream>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{
    enum class Protocol {v2024_11_05, v2025_03_26, v2025_06_18, v2025_11_25};

    class MCPCore;
    struct Context
    {
            static ThorsAnvil::Serialize::PrinterConfig    outputConfig;
            static JsonRPC::OptRequestId                   defaultId;

            Protocol                protocol;
            std::istream&           input;
            std::ostream&           output;
            JsonRPC::OptRequestId   requestId;
            bool                    stream;

            Context(std::istream& input, std::ostream& output, Protocol protocol = Protocol::v2025_11_25);
            virtual ~Context();

            virtual void stop() const = 0;
            virtual std::ostream& addItem() = 0;

            void serverSideStream();
            void setId(JsonRPC::OptRequestId const& id);
            void error(int code, std::string_view message)
            {
                addItem() << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, requestId}, outputConfig);
            }

            template<typename T>
            void  addItem(T const& value)
            {
                if (requestId.has_value()) {
                    addItem() << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{value, requestId}, outputConfig);
                }
            }
    };
}

#endif
