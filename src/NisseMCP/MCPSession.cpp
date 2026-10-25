#include "MCPSession.h"
#include <thread>

namespace ThorsAnvil::Nisse::MCP
{

NISSEMCP_HEADER_ONLY_INCLUDE
void mcpLog(void* userData, const loguru::Message& message)
{
    using ThorsAnvil::Nisse::MCP::MCPSession;
    MCPSession& session = *reinterpret_cast<MCPSession*>(userData);
    session.log(message);
}

NISSEMCP_HEADER_ONLY_INCLUDE
void mcpLogClose(void* userData)
{
    using ThorsAnvil::Nisse::MCP::MCPSession;
    MCPSession& session = *reinterpret_cast<MCPSession*>(userData);
    session.logClose();
}

NISSEMCP_HEADER_ONLY_INCLUDE
void mcpLogFlush(void* userData)
{
    using ThorsAnvil::Nisse::MCP::MCPSession;
    MCPSession& session = *reinterpret_cast<MCPSession*>(userData);
    session.logFlush();
}
}

using namespace ThorsAnvil::Nisse::MCP;

NISSEMCP_HEADER_ONLY_INCLUDE
MCPSession::MCPSession(NameRet&& nameRetriever, ProtocolRet&& protocolRetriever, SessionState initialState, Protocol defaultProtocol, boost::uuids::uuid id)
    : state{initialState}
    , protocol{defaultProtocol}
    , id{std::move(id)}
    , lastUsed{Clock::now()}
    , nameRetriever{std::move(nameRetriever)}
    , protocolRetriever{std::move(protocolRetriever)}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
MCPSession::~MCPSession()
{
    if (logging.hasBeenSet) {
        loguru::remove_callback(toString().c_str());
    }
    // TODO
    // Must close all open connections.
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPSession::initialize(Protocol newProtocol)
{
    state = Confirmed;
    protocol = newProtocol;
}

#ifdef THOR_COVERAGE
#define LOG_CALLBACK    UnitTest::mcpLogUnitTest
#else
#define LOG_CALLBACK    mcpLog
#endif

void MCPSession::setLogging(LoggingLevel level)
{
    if (logging.hasBeenSet) {
        loguru::remove_callback(toString().c_str());
    }
    if (level != LoggingLevel::emergency) {
        logging.hasBeenSet = true;
        loguru::add_callback(toString().c_str(), LOG_CALLBACK, this, static_cast<loguru::Verbosity>(level), mcpLogClose, mcpLogFlush);
    }
    else {
        logging.hasBeenSet = false;
    }
}

void MCPSession::requestAdd(std::thread::id id)
{
    if (logging.hasBeenSet) {
        logging.knownThreads.insert(id);
    }
}

void MCPSession::requestRemove(std::thread::id id)
{
    if (logging.hasBeenSet) {
        logging.knownThreads.erase(id);
    }
}

bool MCPSession::log(const loguru::Message& message)
{
    if (logging.knownThreads.find(std::this_thread::get_id()) != std::end(logging.knownThreads)) {
        std::cerr << "This is a log-able Message: " << message.message << "\n";
        return true;
    }
    return false;
}

void MCPSession::logClose()
{
}

void MCPSession::logFlush()
{
}
