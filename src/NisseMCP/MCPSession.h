#ifndef THORSANVIL_NISSE_MCP_MCPSESSION_H
#define THORSANVIL_NISSE_MCP_MCPSESSION_H

#include "NisseMCPConfig.h"
#include "MCPUtil.h"
#include "Session.h"

#include <boost/uuid.hpp>
#include <string_view>
#include <string>
#include <functional>

namespace ThorsAnvil::Nisse::MCP
{

class Context;

using NameRet       = std::function<std::string_view()>;
using ProtocolRet   = std::function<ProtocolRange()>;

class MCPSession: public Session
{
    SessionState        state;
    Protocol            protocol;
    boost::uuids::uuid  id;
    Time                lastUsed;
    NameRet             nameRetriever;
    ProtocolRet         protocolRetriever;

    public:
        MCPSession(NameRet&& nameRetriever, ProtocolRet&& protocolRetriever, SessionState initialState, Protocol defaultProtocol, boost::uuids::uuid id)
            : state{initialState}
            , protocol{defaultProtocol}
            , id{std::move(id)}
            , lastUsed{Clock::now()}
            , nameRetriever{std::move(nameRetriever)}
            , protocolRetriever{std::move(protocolRetriever)}
        {}
        ~MCPSession()
        {
            // Must close all open connections.
        }

        // Can not be moved or copied.
        // Created in place in the SessionMap only.
        MCPSession(MCPSession const&)                 = delete;
        MCPSession(MCPSession&&)                      = delete;
        MCPSession& operator=(MCPSession const&)      = delete;
        MCPSession& operator=(MCPSession&&)           = delete;

        virtual std::string_view    serviceName()           const override {return nameRetriever();}
        virtual bool                supportBatchRequest()   const override {return protocol < Protocol::v2025_06_18;}
        virtual ProtocolRange       protocolRange()         const override {return protocolRetriever();}

        bool        isValid()                               const {return state != Invalid;}
        bool        isRequested()                           const {return state == Requested;}
        Protocol    getProtocol()                           const {return protocol;}
        std::string toString()                              const {return boost::uuids::to_string(id);}
        bool        hasTimedOut(Duration timeout)           const {return (Clock::now() - lastUsed) > timeout; }

        void     initialize(Protocol newProtocol)  {state = Confirmed; protocol = newProtocol;}
};

using SessionMap = std::map<boost::uuids::uuid, MCPSession>;

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPSession.source"
#endif

#endif
