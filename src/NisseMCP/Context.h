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
    class Server;
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

        bool handleInputStreamWithBatch(Server& server);
        public:
            Context(std::istream& input, std::ostream& output, Protocol protocol = Protocol::v2025_11_25);
            ~Context();

            void stop();
            bool handleInputStream(Server& server);
            void error(int code, std::string_view message, JsonRPC::OptRequestId const& id);
            void setId(IdRef id);
            void serverSideStream();

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
