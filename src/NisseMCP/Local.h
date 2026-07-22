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

template<typename Core>
class Local: public Core
{
    public:
        void run(Protocol minProtocol, std::istream& input, std::ostream& output)
        {
            while (true)
            {
                ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
                LocalContext    context(input, output, minProtocol);
                if (!Core::handleInputStream(context)) {
                    break;
                }
            }
        }
};

}

#endif
