#ifndef THORSANVIL_NISSE_MCP_CORE_H
#define THORSANVIL_NISSE_MCP_CORE_H

#include "NisseMCPConfig.h"
#include "JsonRPCCore.h"

#include "CommandInitialize.h"
#include "CommandLogging.h"

namespace ThorsAnvil::Nisse::MCP
{

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

            void ping(Context& context, JsonRPC::OptRequestId id);
            void loggingSetLevel(Context& context, JsonRPC::OptRequestId id, Command::SetLevelRequestParams const& param);
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
