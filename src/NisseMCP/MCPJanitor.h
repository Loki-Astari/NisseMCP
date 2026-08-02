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
        MCPJanitor(SessionMap& sessionMap, Duration sessionTimeout, Duration initHandShake)
            : sessionMap{sessionMap}
            , sessionTimeout{sessionTimeout}
            , initHandShake{initHandShake}
        {}
        virtual void handleRequest(int /*timerId*/) override
        {
            for (auto loop = std::begin(sessionMap); loop != std::end(sessionMap);) {
                bool timeOut = loop->second.hasTimedOut(loop->second.isRequested() ? initHandShake : sessionTimeout);
                if (timeOut) {
                    loop = sessionMap.erase(loop);
                }
                else {
                    ++loop;
                }
            }
        }
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPJanitor.source"
#endif

#endif
