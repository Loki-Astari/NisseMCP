#ifndef THORSANVIL_NISSE_MCP_MCPLOCAL_H
#define THORSANVIL_NISSE_MCP_MCPLOCAL_H

#include "NisseMCPConfig.h"
#include "Local.h"
#include "MCPCore.h"

namespace ThorsAnvil::Nisse::MCP
{

class MCPLocal: public Local
{
    MCPCore     core;
    public:
        virtual JsonRPCCore& getCore()  override   {return core;}
};

}

#endif
