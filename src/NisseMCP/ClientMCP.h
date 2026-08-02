#ifndef THORSANVIL_NISSE_MCP_CLIENT_MCP_H
#define THORSANVIL_NISSE_MCP_CLIENT_MCP_H

#include "NisseMCPConfig.h"
#include "MCPUtil.h"
#include "Context.h"

#include "NisseHTTP/ClientHTTP.h"
#include "ThorSerialize/JsonThor.h"

#include <string>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

struct ClientConfig
{
    std::string_view    slot    = "/mcp";
    Protocol            protocol;
};

class ClientMCP: private ThorsAnvil::Nisse::HTTP::ClientHTTP
{
    using HeaderRequest = ThorsAnvil::Nisse::HTTP::HeaderRequest;

    std::string     slot;
    Protocol        protocol;
    HeaderRequest   headers;
    int             requestId;

    std::string     getNextRequestId() {return "";}

    public:
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketInfo const& info, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1);
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInfo const& info, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1);
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketService const& info, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1);
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketService const& info, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1);

        template<typename Command, typename... Args>
        std::pair<bool, typename Command::UserData> sendRequest(Args&&... args)
        {
            using Request   = typename Command::Request;
            using Result    = typename Command::Result;
            using UserData  = typename Command::UserData;

            bool ok = false;
            Result reply;
            post_async({.path = slot, .headers = headers}, Request{getNextRequestId(), std::forward<Args>(args)...}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
            {
                ok = (resp.getStatus() == 202) && (resp.body() >> ThorsAnvil::Serialize::jsonImporter(reply));
            });
            if (ok && reply.result.has_value()) {
                return {true, reply.result.value()};
            }
            return {false, UserData{}};
        }

        template<typename Command, typename... Args>
        bool sendNotification(Args&&... args)
        {
            using Request   = typename Command::Request;

            bool ok = false;
            post_async({.path = slot, .headers = headers}, Request{{}, std::forward<Args>(args)...}, [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp){ok = resp.getStatus() == 200;});
            return ok;
        }

    private:
        void init(ClientConfig const& config, std::string_view origin);
        virtual bool resetStream() override;
};

}

#endif
