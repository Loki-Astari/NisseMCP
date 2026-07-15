#ifndef THORSANVIL_NISSE_MCP_CONTEXT_H
#define THORSANVIL_NISSE_MCP_CONTEXT_H

#include <ostream>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

    class Context
    {
        public:
            virtual std::ostream&  output() = 0;
    };
}

#endif
