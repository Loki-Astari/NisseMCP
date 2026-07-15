#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "NisseMCPConfig.h"
#include "Server.h"
#include "Context.h"
#include "CommandInitialize.h"

namespace ThorsAnvil::Nisse::MCP
{

    class MCPServer: public Server
    {
        public:
            MCPServer(ServerConfig const& config)
                : Server(config)
            {
                addExecutor("initialize",                [&](Context& context, Command::InitializeRequestParams const& param){return initialize(context, param);});
                addExecutor("notifications/initialized", [&](Context& context){return notifications_Initialized(context);});
            }

            void initialize(Context& context, Command::InitializeRequestParams const& param);
            void notifications_Initialized(Context& context);
    };
}

#endif
