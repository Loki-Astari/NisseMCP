#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "NisseMCPConfig.h"
#include "JsonRPC.h"
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

// Map an `operator()` signature to its single parameter.
// Catch-all: no `Param` member. Only the `P const&` single-parameter forms
// below define `Param`, so anything else (by-value, multi-arg, zero-arg)
// is a clean SFINAE-friendly rejection rather than a hard error.
template<typename F>
struct ParamOfSignature
{};
template<typename R, typename C, typename P>
struct ParamOfSignature<R(C::*)(P const&) const>
{
    using Param = P;
};
template<typename R, typename C, typename P>
struct ParamOfSignature<R(C::*)(P const&)>
{
    using Param = P;
};

// Primary template: for callables (lambdas, functors) deduce from operator().
// Delegating to ParamOfSignature (not FirstParam) means an unrecognised
// signature stops here with no `Param` instead of recursing into a hard error.
template<typename T>
struct FirstParam : ParamOfSignature<decltype(&std::remove_reference_t<T>::operator())>
{};

// Raw function type.
template<typename R, typename P>
struct FirstParam<R(P const&)>
{
    using Param = P;
};
// std::function.
template<typename R, typename P>
struct FirstParam<std::function<R(P const&)>>
{
    using Param = P;
};

template<typename C>
concept HadSingleParam = requires {typename FirstParam<std::remove_reference_t<C>>::Param;};

class Server
{
    static ThorsAnvil::Serialize::PrinterConfig    outputConfig;


    ExecuteMap executeMap;

    public:
        Server(ServerConfig const& config);

        enum State {OK, ErrorReported, ErrorNoInput};

        State   processesStream(std::istream& input, std::ostream& output);
        bool    processFunctionCall(std::istream& input, std::ostream& output, std::size_t& count, std::string_view sep);

        void resource();
        void tool();
        void prompt();

        // Utility Functions:
        JsonRPC::Response loggingSetLevel(SetLevelRequestParams const& level);

        template<typename T>
        using Executor = std::function<JsonRPC::Response(T const&)>;
        using ExecutorVoid = std::function<JsonRPC::Response()>;

        template<typename C, typename P = typename FirstParam<std::remove_reference_t<C>>::Param>
        requires HadSingleParam<C>
        void addExecutor(std::string const& name, C&& callable)
        {
            executeMap[name] = [executor = Executor<P>{std::forward<C>(callable)}](std::string_view view)
            {
                P   param;
                if (!(view >> ThorsAnvil::Serialize::jsonImporter(param))) {
                    return JsonRPC::Response{-32602, "Invalid params"};
                }

                try
                {
                    return executor(param);
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
            executeMap[name] = [executor = ExecutorVoid{std::forward<C>(f)}](std::string_view param)
            {
                if (!param.empty()) {
                    return JsonRPC::Response{-32602, "Invalid params"};
                }
                try
                {
                    return executor();
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
