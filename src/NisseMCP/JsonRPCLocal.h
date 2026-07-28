#ifndef THORSANVIL_NISSE_MCP_JsonRPCLOCAL_H
#define THORSANVIL_NISSE_MCP_JsonRPCLOCAL_H

#include "NisseMCPConfig.h"
#include "Local.h"

namespace ThorsAnvil::Nisse::MCP
{

class JsonRPCLocal: public Local
{
    JsonRPCCore     core;
    public:
        virtual JsonRPCCore& getCore()  override   {return core;}
};

}

#endif
