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

            std::istream&           input;
            JsonRPC::OptRequestId   requestId;
            bool                    stream;

            Context(std::istream& input);
            virtual ~Context();

            virtual void stop() const = 0;
            virtual std::ostream& addItem() = 0;
            virtual void serverSideStream();
            virtual void error(int code, std::string_view message)
            {
                addItem() << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, requestId}, outputConfig);
            }

            void setId(JsonRPC::OptRequestId const& id);
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
