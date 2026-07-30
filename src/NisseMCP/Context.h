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

    class Session;
    class Context
    {
        public:
            Session&                session;
            std::istream&           input;
        protected:
            std::size_t             count;
            bool                    stream;

        public:
            static ThorsAnvil::Serialize::PrinterConfig    outputConfig;

            Context(Session& session, std::istream& input);
            virtual ~Context();

            virtual void serverSideStream();
            virtual void error(JsonRPC::OptRequestId id, int code, std::string_view message);
            virtual void addNote();
            virtual std::ostream& addItem() = 0;

            template<typename T>
            void  addItem(JsonRPC::OptRequestId id, T const& value)
            {
                if (id.has_value()) {
                    addItem() << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{value, id}, outputConfig);
                    ++count;
                }
                else {
                    addNote();
                }
            }
    };
}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "Context.source"
#endif

#endif
