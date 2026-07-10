#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "NisseMCPConfig.h"
#include "Server.h"

namespace ThorsAnvil::Nisse::MCP
{

class Local: public Server
{
    public:
        Local(ServerConfig const& config);

        void run(std::istream& input = std::cin, std::ostream& output = std::cout);
};

}

#endif
