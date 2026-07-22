#ifndef THORSANVIL_NISSE_JSON_RPC_CORE_H
#define THORSANVIL_NISSE_JSON_RPC_CORE_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"
#include "Context.h"
#include "MetaFunction.h"

#include "CommandInitialize.h"

#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"

#include <map>
#include <string>
#include <string_view>
#include <iostream>
#include <functional>
#include <utility>
#include <type_traits>

namespace ThorsAnvil::Nisse::MCP
{

using ExecuteMap = std::map<std::string, std::function<void(Context&, JsonRPC::Request const&)>>;

class JsonRPCCore
{

    ExecuteMap      executeMap;

    public:
        static const ThorsAnvil::Serialize::PrinterConfig    outputConfig;

        virtual bool handleInputStream(Context& context);

    private:
        bool handleInputStreamWithBatch(Context& context);
        bool readOneAction(Context& context);

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

    public:
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
                    context.error(-32602, "Invalid params");
                    context.stop();
                    return;
                }

                try
                {
                    executor(context, param);
                }
                catch (...)
                {
                    context.error(-32603, "Internal error");
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
                    context.error(-32602, "Invalid params");
                    context.stop();
                }
                try
                {
                    executor(context);
                }
                catch (...)
                {
                    context.error(-32603, "Internal error");
                }
            };
        }
};

}

#endif
