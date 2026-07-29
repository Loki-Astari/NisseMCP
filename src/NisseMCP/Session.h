#ifndef THORSANVIL_NISSE_MCP_SESSION_H
#define THORSANVIL_NISSE_MCP_SESSION_H

#include "NisseMCPConfig.h"
#include "Context.h"

#include <boost/uuid.hpp>

#include <ctime>
#include <map>

namespace ThorsAnvil::Nisse::MCP
{

    using Clock     = std::chrono::steady_clock;
    using Time      = std::chrono::time_point<Clock>;
    using Duration  = std::chrono::seconds;

    enum SessionState {Invalid, Requested, Confirmed};

    class Session
    {
        SessionState        state;
        Protocol            protocol;
        boost::uuids::uuid  id;
        Time                lastUsed;

        public:
            Session(SessionState initialState, Protocol defaultProtocol, boost::uuids::uuid id)
                : state{initialState}
                , protocol{defaultProtocol}
                , id{std::move(id)}
                , lastUsed{Clock::now()}
            {}
            ~Session()
            {
                // Must close all open connections.
            }

            // Can not be moved or copied.
            // Created in place in the SessionMap only.
            Session(Session const&)                 = delete;
            Session(Session&&)                      = delete;
            Session& operator=(Session const&)      = delete;
            Session& operator=(Session&&)           = delete;

            bool isValid()      const {return state != Invalid;}
            bool isRequested()  const {return state == Requested;}

            void     initialize(Protocol newProtocol)  {state = Confirmed; protocol = newProtocol;}
            Protocol getProtocol() const {return protocol;}
    };

    using SessionMap = std::map<boost::uuids::uuid, Session>;

}

#endif
