#include "MCPCore.h"

using namespace ThorsAnvil::Nisse::MCP;

MCPCore::MCPCore(MCPCoreConfig const& config)
    : config(config)
{
    addExecutor("initialize",                [&](Context& context, Command::InitializeRequestParams const& param){return initialize(context, param);});
    addExecutor("notifications/initialized", [&](Context& context){return notifications_Initialized(context);});
}

bool MCPCore::readOneAction(Context& context)
{
    JsonRPC::Request    rpc;
    if (!(context.input >> ThorsAnvil::Serialize::jsonImporter(rpc))) {
        context.error(-32700, "Parse error");
        return false;
    }
    context.setId(rpc.id);
    if (rpc.jsonrpc != "2.0") {
        context.error(-32600, "Invalid Request");
        return true;
    }

    auto find = executeMap.find(rpc.method);
    if (find == std::end(executeMap)) {
        context.error(-32601, "Method not found");
        return true;
    }

    (find->second)(context, rpc);
    return context.input.good();
}

bool MCPCore::handleInputStream(Context& context)
{
    if (context.protocol < Protocol::v2025_06_18) {
        return handleInputStreamWithBatch(context);
    }
    else {
        char nextChar;

        if (!(context.input >> nextChar)) {
            // No input.
            // This is probably because this is being called on stream in a loop.
            return false;
        }
        context.input.unget();
        return readOneAction(context);
    }
}

bool MCPCore::handleInputStreamWithBatch(Context& context)
{
    using namespace std::string_view_literals;

    // Peek at first character to see if this is Batch or a single command.
    char nextChar;

    if (!(context.input >> nextChar)) {
        // No input.
        // This is probably because this is being called on stream in a loop.
        return false;
    }

    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        if (!(context.input >> nextChar)) {
            // If input fails then this is a parser error.
            context.error(-32700, "Parse error");
            context.stop();
            return false;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            context.error(-32600, "Invalid Request");
            context.stop();
            return false;
        }
        // Put back the next char we just stole for empty array checks.
        context.input.unget();

        context.serverSideStream();

        while (nextChar != ']')
        {
            if (!readOneAction(context)) {
                // Bad Json. So we are going to exit.
                //           Other types of error allow us to continue.
                return false;
            }
            context.setId({});
            if (!(context.input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                context.error(-32700, "Parse error");
                context.stop();
                return false;
            }
        }
        return context.input.good();
    }
    else {
        // Put back the character we stole doing the check for an array.
        context.input.unget();
        return readOneAction(context);
    }
}

void MCPCore::initialize(Context& context, Command::InitializeRequestParams const& param)
{
    using namespace std::string_literals;
    if (param.protocolVersion != "2025-11-25"s) {
        context.addItem(Command::InitializeResult{_meta: {}, protocolVersion: "2025-11-25"s, capabilities: {logging: {}, completions: {}, prompts: {}, resources: {}, tools: {}, tasks: {}}, serverInfo: {}, instructions: {}});
    }
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
