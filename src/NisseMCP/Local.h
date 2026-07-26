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
        std::ostream&   output;

    public:
        LocalContext(std::istream& input, std::ostream& output);

        ~LocalContext();

        virtual void stop() const override;
        virtual std::ostream& addItem() override;
};

template<typename Core>
class Local
{
    Core& core;
    public:
        Local(Core& core)
            : core{core}
        {}

        void run(std::istream& input, std::ostream& output)
        {
            while (true)
            {
                ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
                LocalContext    context(input, output);
                if (!core.handleInputStream(context)) {
                    break;
                }
            }
        }
};

}

#endif
