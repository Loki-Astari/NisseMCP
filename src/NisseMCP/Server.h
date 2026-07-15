#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"
#include "MetaFunction.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"
#include <iostream>
#include <type_traits>

namespace ThorsAnvil::Nisse::MCP
{

// *****
// https://modelcontextprotocol.io/specification/2025-11-25/schema#common-types
// *****

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logginglevel
enum LoggingLevel /* Vera Ignore */ {debug, info, notice, warning, error, critical, alert, emergency};


// *****
// https://modelcontextprotocol.io/specification/2025-11-25/schema#logging/setlevel
// ******
struct SetLevelRequestParams
{
    LoggingLevel level;
};

/*
 * Server Types;
 */
class ServerConfig
{
};

using ExecuteMap = std::map<std::string, std::function<JsonRPC::Response(std::string_view)>>;

class Server
{
    static ThorsAnvil::Serialize::PrinterConfig    outputConfig;


    ExecuteMap executeMap;

    public:
        Server(ServerConfig const& config);

        enum State {OK, ErrorReported, ErrorNoInput};

        State   processesStream(std::istream& input, std::ostream& output);
        bool    processFunctionCall(std::istream& input, std::ostream& output, std::size_t& count, std::string_view sep);

#if 0
        void resource();
        void tool();
        void prompt();
#endif

        template<typename T>
        using Executor = std::function<JsonRPC::Response(T const&)>;
        using ExecutorVoid = std::function<JsonRPC::Response()>;

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
            executeMap[name] = [executor = std::forward<C>(callable)](std::string_view view) -> JsonRPC::Response
            {
                P   param;
                if (!(view >> ThorsAnvil::Serialize::jsonImporter(param))) {
                    return JsonRPC::Response{-32602, "Invalid params"};
                }

                try
                {
                    return invokeToResponse(executor, param);
                }
                catch (...)
                {
                    return JsonRPC::Response{-32603, "Internal error"};
                }
            };
        }
        template<typename C>
        requires std::invocable<C>
        void addExecutor(std::string const& name, C&& f)
        {
            executeMap[name] = [executor = std::forward<C>(f)](std::string_view param) -> JsonRPC::Response
            {
                if (!param.empty()) {
                    return JsonRPC::Response{-32602, "Invalid params"};
                }
                try
                {
                    return invokeToResponse(executor);
                }
                catch (...)
                {
                    return JsonRPC::Response{-32603, "Internal error"};
                }
            };
        }
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::SetLevelRequestParams, level);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::ServerConfig);

#endif
