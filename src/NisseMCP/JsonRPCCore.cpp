#include "JsonRPCCore.h"

using namespace ThorsAnvil::Nisse::MCP;

bool JsonRPCCore::handleInputStream(Context& context)
{
    if (supportBatchRequest()) {
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

bool JsonRPCCore::readOneAction(Context& context)
{
    JsonRPC::Request    rpc;
    if (!(context.input >> ThorsAnvil::Serialize::jsonImporter(rpc))) {
        context.error({}, -32700, "Parse error");
        return false;
    }
    if (rpc.jsonrpc != "2.0") {
        context.error(rpc.id, -32600, "Invalid Request");
        return true;
    }

    auto find = executeMap.find(rpc.method);
    if (find == std::end(executeMap)) {
        context.error(rpc.id, -32601, "Method not found");
        return true;
    }

    (find->second)(context, rpc);
    return context.input.good();
}

bool JsonRPCCore::handleInputStreamWithBatch(Context& context)
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
            context.error({}, -32700, "Parse error");
            context.input.setstate(std::ios_base::failbit);
            return false;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            context.error({}, -32600, "Invalid Request");
            context.input.setstate(std::ios_base::failbit);
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
            if (!(context.input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                context.error({}, -32700, "Parse error");
                context.input.setstate(std::ios_base::failbit);
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
