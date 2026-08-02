#ifndef THORSANVIL_NISSE_JSON_RPC_SESSION_H
#define THORSANVIL_NISSE_JSON_RPC_SESSION_H

#include "NisseMCPConfig.h"
#include "MCPUtil.h"
#include "Session.h"

#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

class JsonRPCSession: public Session
{
    public:
        virtual std::string_view    serviceName()           const override {return "";}
        virtual bool                supportBatchRequest()   const override {return true;}
        virtual ProtocolRange       protocolRange()         const override {return {Protocol::v2024_11_05, Protocol::v2024_11_05};}
};

}

#endif
