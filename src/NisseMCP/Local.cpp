#include "Local.h"
#include <tuple>

#include "ThorsLogging/ThorsLogging.h"

using namespace ThorsAnvil::Nisse::MCP;

Local::Local(ServerConfig const& config)
    : Server{config}
{}

void Local::run(std::istream& input, std::ostream& output)
{
    while (true)
    {
        ThorsLogInfo("ThorsAnvil::Nisse::MCP::Local", "run", "Command Execution Complete");
        LocalContext    context(input, output, getConfig().minProtocol);
        if (!context.handleInputStream(*this)) {
            break;
        }
    }
}


LocalContext::LocalContext(std::istream& input, std::ostream& output, Protocol protocol)
    : Context(input, output, protocol)
    , count(0)
{}

LocalContext::~LocalContext()
{
    // Close the output array.
    if (stream && count > 0) {
        output << "]";
    }
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void LocalContext::stop()
{
    input.setstate(std::ios_base::failbit);
}

std::ostream& LocalContext::addItem()
{
    std::string_view sep = !stream ? "" : (count == 0) ? "[" : ",";
    ++count;
    return output << sep;
}

bool LocalContext::handleInputStream(Server& server)
{
    if (protocol < Protocol::v2025_06_18) {
        return handleInputStreamWithBatch(server);
    }
    else {
        char nextChar;

        if (!(input >> nextChar)) {
            // No input.
            // This is probably because this is being called on stream in a loop.
            return false;
        }
        input.unget();
        return server.readOneAction(input, *this);
    }
}

bool LocalContext::handleInputStreamWithBatch(Server& server)
{
    using namespace std::string_view_literals;

    // Peek at first character to see if this is Batch or a single command.
    char nextChar;

    if (!(input >> nextChar)) {
        // No input.
        // This is probably because this is being called on stream in a loop.
        return false;
    }

    if (nextChar == '[') {

        // If this is a batch request.
        // Then unpack the batch a command at a time and execute it.

        if (!(input >> nextChar)) {
            // If input fails then this is a parser error.
            error(-32700, "Parse error");
            stop();
            return false;
        }
        if (nextChar == ']') {
            // If this is an empty array then it is an invalid request.
            error(-32600, "Invalid Request");
            stop();
            return false;
        }
        // Put back the next char we just stole for empty array checks.
        input.unget();

        serverSideStream();

        while (nextChar != ']')
        {
            if (!server.readOneAction(input, *this)) {
                // Bad Json. So we are going to exit.
                //           Other types of error allow us to continue.
                return false;
            }
            setId({});
            if (!(input >> nextChar && (nextChar == ',' || nextChar == ']'))) {
                error(-32700, "Parse error");
                stop();
                return false;
            }
        }
        return input.good();
    }
    else {
        // Put back the character we stole doing the check for an array.
        input.unget();
        return server.readOneAction(input, *this);
    }
}

