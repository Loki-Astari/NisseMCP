#ifndef THORSAVNIL_NISSE_MCP_CLIENT_CONTEXT_MCP_H
#define THORSAVNIL_NISSE_MCP_CLIENT_CONTEXT_MCP_H

#include "Context.h"
#include "Session.h"

namespace ThorsAnvil::Nisse::MCP
{

class ClientSession: public Session
{
    public:
        virtual std::string_view    serviceName()           const {return "Client";}
        virtual bool                supportBatchRequest()   const {return false;}
        virtual ProtocolRange       protocolRange()         const {return {Protocol::v2025_11_25, Protocol::v2025_11_25};}

};

class ClientContextMCP: public Context
{
    ClientSession       session;
    std::stringstream   input;

    public:
        ClientContextMCP(std::string val)
            : Context(session, input)
            , session{}
            , input{std::move(val)}
        {}

        virtual std::ostream& addItem()
        {
            std::cerr << "We want to send data to server again. This means we need another post\n";
            std::cerr << "TODO\n";
            return std::cerr;
        }
};

}

#endif
