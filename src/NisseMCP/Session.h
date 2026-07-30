#ifndef THORSANVIL_NISSE_MCP_SESSION_H
#define THORSANVIL_NISSE_MCP_SESSION_H

#include "NisseMCPConfig.h"


namespace ThorsAnvil::Nisse::MCP
{
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
            virtual bool     supportBatchRequest()  const = 0;
    };


}

#endif
