#include "Context.h"
#include "Server.h"

using namespace ThorsAnvil::Nisse::MCP;

ThorsAnvil::Serialize::PrinterConfig    Context::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};
JsonRPC::OptRequestId                   Context::defaultId;

Context::Context(std::istream& input, std::ostream& output, Protocol protocol)
    : protocol(protocol)
    , input(input)
    , output(output)
    , requestId(defaultId)
    , errorState(false)
    , count(0)
    , stream(false)
{}

Context::~Context()
{
    // Close the output array.
    if (stream && count > 0) {
        output << "]";
    }
    if (errorState) {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void Context::stop()
{
    input.setstate(std::ios_base::failbit);
}

void Context::error(int code, std::string_view message, JsonRPC::OptRequestId const& id)
{
    errorState = true;
    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
    ++count;
    output << sep << ThorsAnvil::Serialize::jsonExporter(JsonRPC::Response{code, message, id}, outputConfig);
}

void Context::setId(IdRef id)
{
    requestId = id;
}

void Context::serverSideStream()
{
    stream = true;
}

void Context::handleInputStream(Server& server)
{
    using namespace std::string_view_literals;

    // Peek at first character to see if this is Batch or a single command.
    char nextChar;

    if (!(input >> nextChar)) {
        // No input.
        // This is probably because this is being called on stream in a loop.
        return;
    }

    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        if (!(input >> nextChar)) {
            // If input fails then this is a parser error.
            error(-32700, "Parse error", {});
            stop();
            return;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            error(-32600, "Invalid Request", {});
            stop();
            return;
        }
        // Put back the next char we just stole for empty array checks.
        input.unget();

        serverSideStream();

        while (nextChar != ']')
        {
            if (!server.readOneAction(input, *this)) {
                // Bad Json. So we are going to exit.
                //           Other types of error allow us to continue.
                return;
            }
            if (!(input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                error(-32700, "Parse error", {});
                stop();
                return;
            }
        }
    }
    else {
        // Put back the character we stole doing the check for an array.
        input.unget();
        server.readOneAction(input, *this);
    }
    return;
}

