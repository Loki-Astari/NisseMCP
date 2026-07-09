#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "Server.h"

namespace ThorsAnvil::Nisse::MCP
{

class Local
{
    Server              server;

    public:
        Local(MugServerConfig const& config);

        void run();
};

}

#endif
