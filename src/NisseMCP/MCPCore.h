#ifndef THORSANVIL_NISSE_MCP_CORE_H
#define THORSANVIL_NISSE_MCP_CORE_H

#include "NisseMCPConfig.h"
#include "JsonRPCCore.h"

#include "CommandInitialize.h"

namespace ThorsAnvil::Nisse::MCP
{

class Context;
class MCPCore: public JsonRPCCore
{
    Protocol protocol;
    public:
        MCPCore(Protocol protocol);

    private:
        virtual bool supportBatchRequest() const {return protocol < Protocol::v2025_06_18;}

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
