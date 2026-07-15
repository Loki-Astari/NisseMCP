#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_H

#include "Server.h"
#include "CommandInitialize.h"

namespace ThorsAnvil::Nisse::MCP
{

    class MCPServer: public Server
    {
        public:
            MCPServer(ServerConfig const& config)
                : Server(config)
            {
                addExecutor("initialize",                [&](Command::InitializeRequestParams const& param){return initialize(param);});
                addExecutor("notifications/initialized", [&](){return notifications_Initialized();});
            }

            Command::InitializeResult initialize(Command::InitializeRequestParams const& param);
            void notifications_Initialized();
    };
}

#endif
