#ifndef THORSANVIL_NISSE_MCP_MCPSERVER_CONTEXT_H
#define THORSANVIL_NISSE_MCP_MCPSERVER_CONTEXT_H

#include "NisseMCPConfig.h"
#include "ServerContext.h"
#include "MCPSession.h"

#include "NisseServer/TimerAction.h"
#include "NisseServer/Context.h"

#include <boost/uuid.hpp>

#include <chrono>
#include <thread>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

class MCPServerContext: public ServerContext, public ThorsAnvil::Nisse::Server::ContextThreadNotifyInterface
{
    public:
        MCPServerContext(MCPSession& session, ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
            : ServerContext(session, request, response)
        {}

        MCPSession& getMCPSession() {return dynamic_cast<MCPSession&>(session);}
        virtual ThorsAnvil::Nisse::HTTP::Response& addHeaders(ThorsAnvil::Nisse::HTTP::Response& response) override;

        virtual void notifyThreadJoinContext(std::thread::id id)   override;
        virtual void notifyThreadYieldContext(std::thread::id id)  override;
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPServerContext.source"
#endif

#endif
