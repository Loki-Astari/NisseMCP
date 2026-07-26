#include "MCPCore.h"
#include "JsonRPC.h"
#include "NisseHTTP/Response.h"

using namespace ThorsAnvil::Nisse::MCP;

bool MCPCoreRequestValidtor::validateRequest(ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response, std::string_view originAllowed)
{
    auto const& origin = request.headers().getHeader("origin");

    if (origin.size() != 1 || origin[0] != originAllowed) {
        response.setStatus(403);
        response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{10, "Invalid Origin. Request forbidden", {}});
        return false;
    }

    auto const& accept = request.headers().getHeader("accept");
    bool acceptJson = false;
    bool acceptStream= false;
    for (auto const& aVal: accept) {
        if (aVal == "application/json") {
            acceptJson = true;
        }
        if (aVal == "text/event-stream") {
            acceptStream = true;
        }
    }
    if (!acceptJson || !acceptStream) {
        response.setStatus(404);
        response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked) << ThorsAnvil::Serialize::jsonExporter(JsonRPC::ClientResponse{11, "Invalid Accept: Requires 'application/json' and 'text/event-stream'", {}});
        return false;
    }
    return true;
}

MCPCore::MCPCore(Protocol protocol)
    : protocol{protocol}
{
    addExecutor("initialize",                [&](Context& context, Command::InitializeRequestParams const& param){return initialize(context, param);});
    addExecutor("notifications/initialized", [&](Context& context){return notifications_Initialized(context);});
}

void MCPCore::initialize(Context& context, Command::InitializeRequestParams const& /*param*/)
{
    using namespace std::string_literals;
    context.addItem(Command::InitializeResult{
                                                ._meta          = {},
                                                .protocolVersion= "2025-11-25"s,
                                                .capabilities =
                                                {
                                                        .logging        = {},
                                                        .completions    = {},
                                                        .prompts        = {},
                                                        .resources      = {},
                                                        .tools          = {},
                                                        .tasks          = {}
                                                },
                                                .serverInfo     = {},
                                                .instructions   = {}
                                             });
}

void MCPCore::notifications_Initialized(Context& /*context*/)
{
}
#if 0
void MCPCore::resource()
{
}

void MCPCore::tool()
{
}

void MCPCore::prompt()
{
}
#endif
