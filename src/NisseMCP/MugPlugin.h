#ifndef THORSANVIL_NISSE_MCP_MUG_H
#define THORSANVIL_NISSE_MCP_MUG_H

#include "ThorsMug/MugPlugin.h"
#include "Server.h"

namespace ThorsAnvil::Nisse::MCP
{

class MugPlugin: public ThorsAnvil::ThorsMug::MugPluginSimple
{
    Server              server;

    // Send request to be handeled by the server.
    void processesRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response);
    public:
        MugPlugin(ServerConfig const& config);

        // Register handlers with mug server.
        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction();
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
