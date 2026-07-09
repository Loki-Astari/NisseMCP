#ifndef THORSANVIL_NISSE_MCP_JSON_RCP_H
#define THORSANVIL_NISSE_MCP_JSON_RCP_H

#include <istream>

// https://www.jsonrpc.org/specification

namespace ThorsAnvil::NisseMCP
{
    class JsonRPC
    {
        public:
            JsonRPC(std::istream& requestStream);
    };
}

#endif
