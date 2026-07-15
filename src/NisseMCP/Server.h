#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"
#include "Context.h"
#include "MetaFunction.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"
#include <iostream>
#include <type_traits>

namespace ThorsAnvil::Nisse::MCP
{

class ServerConfig
{
};

using ExecuteMap = std::map<std::string, std::function<void(Context&, JsonRPC::Request const&)>>;

class Server
{
    static ThorsAnvil::Serialize::PrinterConfig    outputConfig;


    ExecuteMap executeMap;

        bool    processFunctionCall(std::istream& input, Context& context);
    public:
        Server(ServerConfig const& config);

        void   processesStream(std::istream& input, Context& context);

#if 0
        void resource();
        void tool();
        void prompt();
#endif

        template<typename F, typename... Args>
        static JsonRPC::Response invokeToResponse(F&& f, Args&&... args)
        {
            if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
                std::forward<F>(f)(std::forward<Args>(args)...);
                return JsonRPC::Response{};
            }
            else {
                return JsonRPC::Response(std::forward<F>(f)(std::forward<Args>(args)...));
            }
        }

        template<typename C, typename P = typename FirstParam<std::remove_reference_t<C>>::Param>
        requires HadSingleParam<C>
        void addExecutor(std::string const& name, C&& callable)
        {
            executeMap[name] = [executor = std::forward<C>(callable)](Context& context, JsonRPC::Request const& rpc)
            {
                using namespace std::string_view_literals;
                std::string_view view = rpc.params.has_value() ? rpc.params->getView() : ""sv;
                P                param;
                if (!(view >> ThorsAnvil::Serialize::jsonImporter(param))) {
                    context.error(-32602, "Invalid params", rpc.id);
                    context.stop();
                    return;
                }

                try
                {
                    context.setId(rpc.id);
                    executor(context, param);
                }
                catch (...)
                {
                    context.error(-32603, "Internal error", rpc.id);
                }
            };
        }
        template<typename C>
        requires std::invocable<C, Context&>
        void addExecutor(std::string const& name, C&& f)
        {
            executeMap[name] = [executor = std::forward<C>(f)](Context& context, JsonRPC::Request const& rpc)
            {
                if (rpc.params.has_value()) {
                    context.error(-32602, "Invalid params", rpc.id);
                    context.stop();
                }
                try
                {
                    context.setId(rpc.id);
                    executor(context);
                }
                catch (...)
                {
                    context.error(-32603, "Internal error", rpc.id);
                }
            };
        }
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::ServerConfig);

#endif
