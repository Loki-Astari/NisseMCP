#ifndef THORSANVIL_NISSE_MCP_MUG_H
#define THORSANVIL_NISSE_MCP_MUG_H

#include "NisseMCPConfig.h"
#include "MCPCore.h"

#include "ThorsMug/MugPlugin.h"

namespace ThorsAnvil::Nisse::MCP
{

class MugPlugin: public ThorsAnvil::ThorsMug::MugPluginSimple
{
    MCPCore              server;

    // Send request to be handeled by the server.
    void processesRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response);
    public:
        // Register handlers with mug server.
        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction();
};

}

/*
 * Current NisseMCP MCPCore is a Simple Mug Server.
 * Thus we simply redefine THORS_ANVIL_NISSE_MCP_SERVER_INIT => THORS_ANVIL_SIMPLE_MUG_SERVER_INIT at this time.
 * Note:
 * This may change. But functionality will be maintained.
 */
#define THORS_ANVIL_NISSE_MCP_SERVER_INIT(Config, MCPCore)           THORS_ANVIL_SIMPLE_MUG_SERVER_INIT(Config, MCPCore)

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPPlugin.source"
#endif

#endif
