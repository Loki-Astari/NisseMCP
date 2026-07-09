#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "JsonRPC.h"
#include "ThorSerialize/Traits.h"
#include <iostream>

namespace ThorsAnvil::Nisse::MCP
{

class MugServerConfig
{
};

class Server
{
    static ThorsAnvil::Serialize::PrinterConfig    outputConfig;

    public:
        Server(MugServerConfig const& config);

        bool                processesStream(std::istream& input, std::ostream& output);
        void                processFunctionCall(std::istream& input, std::ostream& output);
        JsonRPC::Response   execute(JsonRPC::Request const& request);

        void resource();
        void tool();
        void prompt();
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::MugServerConfig);

#endif
