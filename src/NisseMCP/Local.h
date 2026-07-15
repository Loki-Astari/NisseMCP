#ifndef THORSANVIL_NISSE_MCP_LOCAL_H
#define THORSANVIL_NISSE_MCP_LOCAL_H

#include "NisseMCPConfig.h"
#include "Server.h"
#include "Context.h"

namespace ThorsAnvil::Nisse::MCP
{

class LocalContext: public Context
{
    private:
        std::size_t     count;

        bool handleInputStreamWithBatch(Server& server);
    public:
        LocalContext(std::istream& input, std::ostream& output, Protocol protocol);

        ~LocalContext();

        virtual void stop() override;
        virtual bool handleInputStream(Server& server) override;
        virtual std::ostream& addItem() override;
};

class Local: public Server
{
    public:
        Local(ServerConfig const& config);

        void run(std::istream& input = std::cin, std::ostream& output = std::cout);
};

}

#endif
