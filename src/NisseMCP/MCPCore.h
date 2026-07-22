#ifndef THORSANVIL_NISSE_MCP_CORE_H
#define THORSANVIL_NISSE_MCP_CORE_H

#include "NisseMCPConfig.h"
#include "JsonRPCCore.h"
#include "Context.h"
#include "MetaFunction.h"

#include "CommandInitialize.h"

#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"

#include <map>
#include <string>
#include <string_view>
#include <iostream>
#include <functional>
#include <utility>
#include <type_traits>

namespace ThorsAnvil::Nisse::MCP
{

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
            void initialize(Context& context, Command::InitializeRequestParams const& param);
            void notifications_Initialized(Context& context);
#if 0
        void resource();
        void tool();
        void prompt();
#endif

};

}

#endif
