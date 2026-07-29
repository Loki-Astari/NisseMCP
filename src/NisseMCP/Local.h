#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "NisseMCPConfig.h"
#include "Context.h"

#include <iostream>

namespace ThorsAnvil::Nisse::MCP
{

class JsonRPCCore;

class LocalContext: public Context
{
    private:
        std::ostream&   output;

    public:
        LocalContext(std::istream& input, std::ostream& output);

        ~LocalContext();

        virtual std::ostream& addItem() override;
};

class Local
{
    public:
        virtual ~Local();
        virtual JsonRPCCore& getCore()  = 0;

        void run(std::istream& input, std::ostream& output);
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "Local.source"
#endif

#endif
