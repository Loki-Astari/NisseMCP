#ifndef THORSANVIL_NISSE_MCP_MUG_H
#define THORSANVIL_NISSE_MCP_MUG_H

#include "ThorsMug/MugPlugin.h"

namespace ThorsAnvil::NisseMCP
{

struct MugPluginConfig
{
    bool    init    = false;
};

class MugPlugin: public ThorsAnvil::ThorsMug::MugPluginSimple
{
    MugPluginConfig     config;
    public:
        MugPlugin(MugPluginConfig const& config);

        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction();

        void resource();
        void tool();
        void prompt();
};

}

/*
 * Current NisseMCP Server is a Simple Mug Server.
 * Thus we simply redefine THORS_ANVIL_NISSE_MCP_SERVER_INIT => THORS_ANVIL_SIMPLE_MUG_SERVER_INIT at this time.
 * Note:
 * This may change. But functionality will be maintained.
 */
#define THORS_ANVIL_NISSE_MCP_SERVER_INIT(Config, Server)           THORS_ANVIL_SIMPLE_MUG_SERVER_INIT(Config, Server)
#endif
