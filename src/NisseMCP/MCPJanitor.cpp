#include "MCPJanitor.h"

using namespace ThorsAnvil::Nisse::MCP;


NISSEMCP_HEADER_ONLY_INCLUDE
MCPJanitor::MCPJanitor(SessionMap& sessionMap, Duration sessionTimeout, Duration initHandShake)
    : sessionMap{sessionMap}
    , sessionTimeout{sessionTimeout}
    , initHandShake{initHandShake}
{}

NISSEMCP_HEADER_ONLY_INCLUDE
void MCPJanitor::handleRequest(int /*timerId*/)
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
