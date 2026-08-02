#ifndef THORSANVIL_NISSE_MCP_SERVER_CONTEXT_H
#define THORSANVIL_NISSE_MCP_SERVER_CONTEXT_H

#include "NisseMCPConfig.h"

#include "JsonRPC.h"
#include "Context.h"

#include <NisseHTTP/Request.h>
#include <NisseHTTP/Response.h>

#include <string_view>
#include <iostream>

namespace ThorsAnvil::Nisse::MCP
{

class ServerContext: public Context
{
    public:
    ThorsAnvil::Nisse::HTTP::Request const&   request;
    private:
    ThorsAnvil::Nisse::HTTP::Response&  response;
    std::ostream*                       body;
    int                                 status;
    public:
        ServerContext(Session& session, ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response);
        ~ServerContext();

        virtual void serverSideStream() override;
        virtual void error(JsonRPC::OptRequestId id, int code, std::string_view message) override;
        virtual void addNote() override;
        virtual std::ostream& addItem() override;
    protected:
        virtual ThorsAnvil::Nisse::HTTP::Response& addHeaders(ThorsAnvil::Nisse::HTTP::Response& response) {return response;}
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "ServerContext.source"
#endif

#endif
