#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "NisseMCPConfig.h"
#include "Server.h"
#include "Context.h"

namespace ThorsAnvil::Nisse::MCP
{

class LocalContext: public Context
{
    std::ostream&       out;
    public:
        LocalContext(std::ostream& out)
            : out(out)
        {}
        virtual std::ostream&  output()     override {return out;}
};

class Local: public Server
{
    public:
        Local(ServerConfig const& config);

        void run(std::istream& input = std::cin, std::ostream& output = std::cout);
};

}

#endif
