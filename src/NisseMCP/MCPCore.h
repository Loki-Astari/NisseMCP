#ifndef THORSANVIL_NISSE_MCP_CORE_H
#define THORSANVIL_NISSE_MCP_CORE_H

#include "NisseMCPConfig.h"
#include "JsonRPCCore.h"

#include "CommandInitialize.h"

#include <boost/uuid.hpp>

namespace ThorsAnvil::Nisse::MCP
{

    class Context;

    enum SessionState   {Invalid, Requested, Confirmed};

    using Clock     = std::chrono::steady_clock;
    using Time      = std::chrono::time_point<Clock>;
    using Duration  = std::chrono::seconds;

    class MCPSession: public Session
    {
        SessionState        state;
        Protocol            protocol;
        boost::uuids::uuid  id;
        Time                lastUsed;

        public:
            MCPSession(SessionState initialState, Protocol defaultProtocol, boost::uuids::uuid id)
                : state{initialState}
                , protocol{defaultProtocol}
                , id{std::move(id)}
                , lastUsed{Clock::now()}
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

            virtual bool     supportBatchRequest()  const {return protocol < Protocol::v2025_06_18;}
            bool     isValid()              const {return state != Invalid;}
            bool     isRequested()          const {return state == Requested;}
            Protocol getProtocol()          const {return protocol;}

            void     initialize(Protocol newProtocol)  {state = Confirmed; protocol = newProtocol;}
    };

    class MCPCore: public JsonRPCCore
    {
        public:
            MCPCore();

        private:
            virtual bool supportBatchRequest(Context& context) const;

            // ****************************
            // Handle Individual Commands.
            // ****************************
                // Initialization.
                void initialize(Context& context, JsonRPC::OptRequestId id, Command::InitializeRequestParams const& param);
                void notifications_Initialized(Context& context);
    #if 0
            void resource();
            void tool();
            void prompt();
    #endif

    };

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPCore.source"
#endif

#endif
