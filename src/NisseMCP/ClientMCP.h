#ifndef THORSANVIL_NISSE_MCP_CLIENT_MCP_H
#define THORSANVIL_NISSE_MCP_CLIENT_MCP_H

#include "NisseMCPConfig.h"
#include "MCPUtil.h"
#include "Context.h"
#include "JsonRPC.h"
#include "SSEInfo.h"
#include "ConnectionPool.h"

#include "NisseHTTP/Util.h"
#include "NisseHTTP/ClientHTTP.h"
#include "ThorSerialize/JsonThor.h"

#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace ThorsAnvil::Nisse::MCP
{

struct ClientConfig
{
    std::string_view    slot    = "/mcp";
    Protocol            protocol;
};


class ClientMCP
{
    using HeaderRequest = ThorsAnvil::Nisse::HTTP::HeaderRequest;
    using SocketInit    = ThorsAnvil::ThorsSocket::SocketInit;
    using ClientHTTP    = ThorsAnvil::Nisse::HTTP::ClientHTTP;
    using Version       = ThorsAnvil::Nisse::HTTP::Version;

    ClientConfig                config;
    ConnectionPool              pool;
    HeaderRequest               headers;
    int                         requestId;

    int     getNextRequestId() {return requestId++;}

    enum Init {Constructor};

    ClientMCP(Init, ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInit const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version);
    public:
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketInfo const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1)
            : ClientMCP(Init::Constructor, config, socketInfo, version)
        {}
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInfo const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1)
            : ClientMCP(Init::Constructor, config, socketInfo, version)
        {}
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketService const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1)
            : ClientMCP(Init::Constructor, config, socketInfo, version)
        {}
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SSocketService const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1)
            : ClientMCP(Init::Constructor, config, socketInfo, version)
        {}
        ClientMCP(ClientConfig const& config, ThorsAnvil::ThorsSocket::SocketInit const& socketInfo, ThorsAnvil::Nisse::HTTP::Version version = ThorsAnvil::Nisse::HTTP::Version::HTTP1_1)
            : ClientMCP(Init::Constructor, config, socketInfo, version)
        {}

        template<typename C>
        using Action = std::function<void(typename C::UserData&&)>;
        using Error  = std::function<void(int, int, std::string_view)>;

        template<typename Command>
        void sendRequest(Action<Command>&& action,
                         Error&& errorAction = [](int s, int code, std::string_view m){logErrorMessage(s, code, m);})
        {
            using Request   = typename Command::Request;
            ConnectionFromPool  client{pool};

            client->post_async({std::string{config.slot}, headers}, Request{getNextRequestId()},
                              [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp) {handleRespFromServer<Command>(std::forward<Action<Command>>(action), std::forward<Error>(errorAction), resp);});
        }
        template<typename Command>
        void sendRequest(typename Command::UserParam const& param,
                         Action<Command>&& action,
                         Error&& errorAction = [](int s, int code, std::string_view m){logErrorMessage(s, code, m);})
        {
            using Request   = typename Command::Request;
            ConnectionFromPool  client{pool};

            client->post_async({std::string{config.slot}, headers}, Request{getNextRequestId(), param},
                              [&](ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp) {handleRespFromServer<Command>(std::forward<Action<Command>>(action), std::forward<Error>(errorAction), resp);});
        }

    private:
        void handleErrorRespFromServer(Error&& errorAction, ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
        {
            JsonRPC::ErrorResponse errorResp;
            if (resp.body() >> ThorsAnvil::Serialize::jsonImporter(errorResp)) {
                std::forward<Error>(errorAction)(resp.getStatus(), errorResp.error.code, errorResp.error.message);
            }
            else {
                std::forward<Error>(errorAction)(resp.getStatus(), 100, "Failed to decode JsonRPC object: Error Path");
            }
        }
        template<typename Command>
        void handleRespFromServer(Action<Command>&& action, Error&& errorAction, ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
        {
            if (resp.getStatus() != 202) {
                handleErrorRespFromServer(std::forward<Error>(errorAction), resp);
                return;
            }

            enum ReponseType {Unknown, ResponseObject, ResponseStream, NoResponse};
            ReponseType  responseType = Unknown;
            // Is this a notification?
            // If so then there is no response object.
            if (responseType == NoResponse) {
                // TODO
                // We should still call the succ/error functions.
                // Need a use case
                return;
            }
            auto const& contentType = resp.getHeader().getHeader("Content-Type");
            if (contentType.size() == 1) {
                if (contentType[0] == "application/json") {
                    responseType = ResponseObject;
                }
                if (contentType[0] == "text/event-stream") {
                    responseType = ResponseStream;
                }
            }

            switch (responseType)
            {
                case Unknown:
                    return std::forward<Error>(errorAction)(resp.getStatus(), 100, "Invalid Content-Type from server");

                case ResponseObject:
                    return handleObjectRespFromServer<Command>(std::forward<Action<Command>>(action), std::forward<Error>(errorAction), resp);

                case ResponseStream:
                    return handleStreamRespFromServer<Command>(std::forward<Action<Command>>(action), std::forward<Error>(errorAction), resp);

                case NoResponse:
                    // TODO
                    throw std::runtime_error("Should not reach here");
            }
        }

        template<typename Command>
        void handleObjectRespFromServer(Action<Command>&& action, Error&& errorAction, ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
        {
            using Result = typename Command::Result;
            Result reply;
            if (!(resp.body() >> ThorsAnvil::Serialize::jsonImporter(reply))) {
                std::forward<Error>(errorAction)(202, 100, "Failed to decode JsonRPC object: Normal Path");
                return;
            }
            if (reply.error.has_value()) {
                std::forward<Error>(errorAction)(202, reply.error.value().code, reply.error.value().message);
                return;
            }
            if (!reply.result.has_value()) {
                std::forward<Error>(errorAction)(202, 100, "No value in the result field");
                return;
            }

            // All checks passed. We have a value.
            std::forward<Action<Command>>(action)(std::move(reply.result.value()));
        }

        template<typename Command>
        void handleStreamRespFromServer(Action<Command>&& action, Error&& errorAction, ThorsAnvil::Nisse::HTTP::ClientHTTPResponse const& resp)
        {
            using Result = typename Command::Result;

            SSEInfo<Result>     event;
            while (resp.body() >> event) {
                if (event.data.error.has_value()) {
                    std::forward<Error>(errorAction)(202, event.data.error.value().code, event.data.error.value().message);
                    continue;
                }
                if (event.data.result.has_value()) {
                    std::forward<Action<Command>>(action)(std::move(event.data.result.value()));
                }
            }
        }

        static void logErrorMessage(int status, int code, std::string_view message);
};

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "ClientMPC.source"
#endif

#endif
