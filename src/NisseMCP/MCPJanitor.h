#ifndef THORSANVIL_NISSE_MCP_MCPJANITOR_H
#define THORSANVIL_NISSE_MCP_MCPJANITOR_H

#include "NisseMCPConfig.h"
#include "MCPSession.h"

#include "NisseServer/TimerAction.h"

#include <chrono>

namespace ThorsAnvil::Nisse::MCP
{

class MCPJanitor: public ThorsAnvil::Nisse::Server::TimerAction
{
    SessionMap&     sessionMap;
    Duration        sessionTimeout;
    Duration        initHandShake;
    public:
        MCPJanitor(SessionMap& sessionMap, Duration sessionTimeout, Duration initHandShake);
        virtual void handleRequest(int /*timerId*/) override;
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPJanitor.source"
#endif

#endif
