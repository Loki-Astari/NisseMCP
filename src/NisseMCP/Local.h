#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "NisseMCPConfig.h"

#include <iostream>

namespace ThorsAnvil::Nisse::MCP
{

class JsonRPCCore;
class Session;

class Local
{
    public:
        virtual ~Local();
        virtual JsonRPCCore& getCore()  = 0;

        void run(Session& session, std::istream& input, std::ostream& output);
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "Local.source"
#endif

#endif
