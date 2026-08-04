#include "SSEInfo.h"

#include <ios>

using namespace ThorsAnvil::Nisse::MCP;

/*
 * Everything below works on the std::streambuf rather than the std::istream.
 *
 * This is done for effeciency. The readEvent() function sets the sentry object and
 * then manipulates the streambuffer rather than the stream.
 *
 */

// A '\r' has just been consumed; take the '\n' of a "\r\n" pair with it.
NISSEMCP_HEADER_ONLY_INCLUDE
void SSEInfoBase::consumePairedLF(std::streambuf& buffer)
{
    if (buffer.sgetc() == '\n') {
        buffer.sbumpc();
    }
}

// Discard the rest of the line, including its terminator.
NISSEMCP_HEADER_ONLY_INCLUDE
void SSEInfoBase::skipLine(std::streambuf& buffer)
{
    for (int c = buffer.sbumpc(); c != endOfFile; c = buffer.sbumpc())
    {
        if (c == '\n') {
            return;
        }
        if (c == '\r') {
            consumePairedLF(buffer);
            return;
        }
    }
}

// Read the rest of the line into dst, consuming its terminator.
// SSE accepts "\r\n", "\n" or a bare "\r", which is why std::getline does not fit.
NISSEMCP_HEADER_ONLY_INCLUDE
void SSEInfoBase::readLine(std::streambuf& buffer, std::string& dst)
{
    for (int c = buffer.sbumpc(); c != endOfFile; c = buffer.sbumpc())
    {
        if (c == '\n') {
            return;
        }
        if (c == '\r') {
            consumePairedLF(buffer);
            return;
        }
        dst += static_cast<char>(c);
    }
}

/*
 * Read a field name up to its ':'.
 * Note I: A missing ':' is treated as a field name with no value.
 */
NISSEMCP_HEADER_ONLY_INCLUDE
SSEInfoBase::LineKind SSEInfoBase::readFieldName(std::streambuf& buffer, std::string& name)
{
    name.clear();
    for (int c = buffer.sgetc(); ; c = buffer.sgetc())
    {
        switch (c)
        {
            case endOfFile:
                return name.empty() ? LineKind::EndOfInput : LineKind::Field;
            case '\r':
                if (!name.empty()) {
                    return LineKind::Field;
                }
                buffer.sbumpc();
                consumePairedLF(buffer);
                return LineKind::Blank;
            case '\n':
                if (!name.empty()) {
                    return LineKind::Field;
                }
                buffer.sbumpc();
                return LineKind::Blank;
            case ':':
                buffer.sbumpc();
                if (name.empty()) {
                    return LineKind::Comment;
                }
                // Exactly one leading space belongs to the framing, not to the value.
                if (buffer.sgetc() == ' ') {
                    buffer.sbumpc();
                }
                return LineKind::Field;
            default:
                buffer.sbumpc();
                name += static_cast<char>(c);
        }
    }
}

NISSEMCP_HEADER_ONLY_INCLUDE
std::istream& SSEInfoBase::readEvent(std::istream& stream)
{
    // One sentry for the whole event instead of one per character. Whitespace must not
    // be skipped - a blank line is the event boundary, not something to step over.
    std::istream::sentry    guard{stream, true};
    if (!guard) {
        return stream;
    }

    std::streambuf&         buffer      = *stream.rdbuf();
    std::string             name;
    std::string             dataBuffer;             // Store multi lines of data
    bool                    haveData    = false;

    for (;;)
    {
        switch (readFieldName(buffer, name))
        {
            case LineKind::EndOfInput:
                // "If the file ends in the middle of an event, before the final empty
                // line, the incomplete event is not dispatched." Anything collected so
                // far is dropped by virtue of never being swapped out to the caller.
                stream.setstate(std::ios::eofbit | std::ios::failbit);
                return stream;

            case LineKind::Blank:
                if (haveData) {
                    if (!readData(dataBuffer)) {
                        // If there is an unrecoverable error on the input set the failbit
                        stream.setstate(std::ios::failbit);
                    }
                    return stream;              // Complete: hand it over.
                }
                // An event with no data is not dispatched, and its type does not carry
                // over to the next one.
                event.clear();
                break;

            case LineKind::Comment:
                skipLine(buffer);
                break;                          // Keep-alives land here.

            case LineKind::Field:
                if (name == "data")
                {
                    haveData = true;
                    if (!dataBuffer.empty()) {
                        dataBuffer.append("\n");
                    }
                    readLine(buffer, dataBuffer);
                }
                else if (name == "event")
                {
                    event.clear();
                    readLine(buffer, event);
                }
                else if (name == "id")
                {
                    std::string value;
                    readLine(buffer, value);
                    // The spec ignores an id containing a NUL.
                    if (value.find('\0') == std::string::npos) {
                        id = std::move(value);
                    }
                }
                else
                {
                    skipLine(buffer);           // "retry" and anything unrecognised.
                }
                break;
        }
    }
}
