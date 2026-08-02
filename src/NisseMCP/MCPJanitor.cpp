#include "MCPJanitor.h"

using namespace ThorsAnvil::Nisse::MCP;


MCPJanitor::MCPJanitor(SessionMap& sessionMap, Duration sessionTimeout, Duration initHandShake)
    : sessionMap{sessionMap}
    , sessionTimeout{sessionTimeout}
    , initHandShake{initHandShake}
{}

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
