#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "JsonRPC.h"
#include <ThorSerialize/Traits.h>
#include <ThorSerialize/SerUtil.h>
#include <ThorSerialize/JsonThor.h>

namespace ThorsAnvil::NisseMCP
{

class MugServerConfig
{
};

class Server
{
    public:
        Server(MugServerConfig const& config);

        JsonRPC::Response execute(JsonRPC::Request const& request);

        void resource();
        void tool();
        void prompt();
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::NisseMCP::MugServerConfig);

#endif
