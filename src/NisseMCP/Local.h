#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "NisseMCPConfig.h"
#include "MCPCore.h"
#include "Context.h"

namespace ThorsAnvil::Nisse::MCP
{

class LocalContext: public Context
{
    private:
        std::size_t     count;

    public:
        LocalContext(std::istream& input, std::ostream& output, Protocol protocol);

        ~LocalContext();

        virtual void stop() const override;
        virtual std::ostream& addItem() override;
};

class Local: public MCPCore
{
    public:
        Local(MCPCoreConfig const& config);

        void run(std::istream& input = std::cin, std::ostream& output = std::cout);
};

}

#endif
