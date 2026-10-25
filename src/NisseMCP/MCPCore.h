#ifndef THORSANVIL_NISSE_MCP_CORE_H
#define THORSANVIL_NISSE_MCP_CORE_H

#include "NisseMCPConfig.h"
#include "JsonRPCCore.h"

#include "MCPRequestInitialize.h"
#include "MCPRequestLogging.h"

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
            void initialize(Context& context, OptSerializableId id, JsonRPC::InitializeRequestParams const& param);
            void notifications_Initialized(Context& context);

            void ping(Context& context, OptSerializableId id);
            void loggingSetLevel(Context& context, OptSerializableId id, JsonRPC::SetLevelRequestParam const& param);
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
