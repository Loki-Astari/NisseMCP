#include "MCPSession.h"


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
    // TODO
    // Must close all open connections.
}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPSession::initialize(Protocol newProtocol)
{
    state = Confirmed;
    protocol = newProtocol;
}
