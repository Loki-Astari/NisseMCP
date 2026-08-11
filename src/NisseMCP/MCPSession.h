#ifndef THORSANVIL_NISSE_MCP_MCPSESSION_H
#define THORSANVIL_NISSE_MCP_MCPSESSION_H

#include "NisseMCPConfig.h"
#include "MCPUtil.h"
#include "Session.h"
#include "CommandLogging.h"

#include <boost/uuid.hpp>
#include <string_view>
#include <string>
#include <thread>
#include <functional>

namespace ThorsAnvil::Nisse::MCP
{

void mcpLog(void* userData, const loguru::Message& message);
void mcpLogClose(void* userData);
void mcpLogFlush(void* userData);

namespace UnitTest
{
void mcpLogUnitTest(void* userData, const loguru::Message& message);
}

class Context;

using NameRet       = std::function<std::string_view()>;
using ProtocolRet   = std::function<ProtocolRange()>;

class MCPSession: public Session
{
    friend void mcpLog(void* userData, const loguru::Message& message);
    friend void UnitTest::mcpLogUnitTest(void* userData, const loguru::Message& message);
    friend void mcpLogClose(void* userData);
    friend void mcpLogFlush(void* userData);
    using LogableThreads = std::set<std::thread::id>;

    // Logging Callbacks.
    bool log(const loguru::Message& message);
    void logClose();
    void logFlush();

    SessionState        state;
    Protocol            protocol;
    boost::uuids::uuid  id;
    Time                lastUsed;
    NameRet             nameRetriever;
    ProtocolRet         protocolRetriever;
    bool                loggingSet;
    LogableThreads      logableThreads;

    public:
        MCPSession(NameRet&& nameRetriever, ProtocolRet&& protocolRetriever, SessionState initialState, Protocol defaultProtocol, boost::uuids::uuid id);
        ~MCPSession();

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

        void initialize(Protocol newProtocol);

        // Set Logging Level.
        void setLogging(Command::LoggingLevel logLevel);
        void requestAdd(std::thread::id id);
        void requestRemove(std::thread::id id);
};

using SessionMap = std::map<boost::uuids::uuid, MCPSession>;

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "MCPSession.source"
#endif

#endif
