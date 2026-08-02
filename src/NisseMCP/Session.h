#ifndef THORSANVIL_NISSE_MCP_SESSION_H
#define THORSANVIL_NISSE_MCP_SESSION_H

#include "NisseMCPConfig.h"

#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

enum class Protocol;
using ProtocolRange = std::pair<Protocol, Protocol>;
// Session Info has been moved to Context.h
class Session
{
    public:
        Session()
        {}
        virtual ~Session()
        {}

        // I know there will be an interface here.
        // But need to develop the app slightly further before this settles down.
        virtual std::string_view    serviceName()           const = 0;
        virtual bool                supportBatchRequest()   const = 0;
        virtual ProtocolRange       protocolRange()         const = 0;
};

}

#endif
