#ifndef THORSANVIL_NISSE_MCP_SERVER_H
#define THORSANVIL_NISSE_MCP_SERVER_H

#include "JsonRPC.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"
#include <iostream>

namespace ThorsAnvil::Nisse::MCP
{

// *****
// https://modelcontextprotocol.io/specification/2025-11-25/schema#common-types
// *****

// https://modelcontextprotocol.io/specification/2025-11-25/schema#logginglevel
enum LoggingLevel {debug, info, notice, warning, error, critical, alert, emergency};


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

        bool                processesStream(std::istream& input, std::ostream& output);
        void                processFunctionCall(std::istream& input, std::ostream& output);
        JsonRPC::Response   execute(JsonRPC::Request const& request);

        void resource();
        void tool();
        void prompt();

        // Utility Functions:
        JsonRPC::Response loggingSetLevel(SetLevelRequestParams const& level);

        template<typename T>
        using Executor = std::function<JsonRPC::Response(T const&)>;

        template<typename T>
        void addExecutor(std::string const& name, Executor<T>&& f)
        {
            executeMap[name] = [executor = std::forward<Executor<T>>(f)](std::string_view view)
            {
                T   param;
                view >> ThorsAnvil::Serialize::jsonImporter(param);

                return executor(param);
            };
        }
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::SetLevelRequestParams, level);
ThorsAnvil_MakeTrait(ThorsAnvil::Nisse::MCP::ServerConfig);

#endif
