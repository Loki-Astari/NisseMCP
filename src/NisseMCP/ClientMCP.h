#ifndef THORSANVIL_NISSE_MCP_CLIENT_MCP_H
#define THORSANVIL_NISSE_MCP_CLIENT_MCP_H

#include "NisseMCPConfig.h"
#include "MCPUtil.h"
#include "Context.h"
#include "JsonRPC.h"

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

        template<typename C>
        using Action = std::function<void(typename C::UserData&&)>;
        using Error  = std::function<void(int, int, std::string_view)>;

        template<typename Command>
        void sendRequest(Action<Command>&& action,
                         Error&& errorAction = [](int s, int code, std::string_view m){logErrorMessage(s, code, m);})
        {
            using Request   = typename Command::Request;

            post_async({.path = slot, .headers = headers}, Request{getNextRequestId()},
                       [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp) {handleRespFromServer<Command>(std::forward<Action<Command>>(action), std::forward<Error>(errorAction), resp);});
        }
        template<typename Command, typename... Args>
        void sendRequest(Args&&... args,
                         Action<Command>&& action,
                         Error&& errorAction = [](int s, int code, std::string_view m){logErrorMessage(s, code, m);})
        {
            using Request   = typename Command::Request;

            post_async({.path = slot, .headers = headers}, Request{getNextRequestId(), std::forward<Args>(args)...},
                       [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp) {handleRespFromServer<Command>(std::forward<Action<Command>>(action), std::forward<Error>(errorAction), resp);});
        }

    private:
        template<typename Command>
        void handleRespFromServer(Action<Command>&& action, Error&& errorAction, ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
        {
            using Result = typename Command::Result;

            if (resp.getStatus() == 202) {
                Result reply;
                if (resp.body() >> ThorsAnvil::Serialize::jsonImporter(reply)) {
                    std::forward<Action<Command>>(action)(std::move(reply.result));
                }
                else {
                    std::forward<Error>(errorAction)(202, 100, "Failed to decode JsonRPC object: Normal Path");
                }
            }
            else {
                JsonRPC::ErrorResponse errorResp;
                if (resp.body() >> ThorsAnvil::Serialize::jsonImporter(errorResp)) {
                    std::forward<Error>(errorAction)(resp.getStatus(), errorResp.error.code, errorResp.error.message);
                }
                else {
                    std::forward<Error>(errorAction)(202, 100, "Failed to decode JsonRPC object: Error Path");
                }
            }
        }

        static void logErrorMessage(int status, int code, std::string_view message);
        void init(ClientConfig const& config, std::string_view origin);
        virtual bool resetStream() override;
};

}

#endif
