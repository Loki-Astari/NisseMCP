#ifndef THORSANVIL_NISSE_MCP_LOCAL_CONTEXT_H
#define THORSANVIL_NISSE_MCP_LOCAL_CONTEXT_H

#include "NisseMCPConfig.h"
#include "Context.h"

#include <iostream>

namespace ThorsAnvil::Nisse::MCP
{

class LocalContext: public Context
{
    private:
        std::ostream&   output;

    public:
        LocalContext(Session& session, std::istream& input, std::ostream& output);

        ~LocalContext();

        virtual std::ostream& addItem() override;
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "LocalContext.source"
#endif

#endif
