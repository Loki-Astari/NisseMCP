#ifndef THORSANVIL_NISSE_MCP_SSEINFO_H
#define THORSANVIL_NISSE_MCP_SSEINFO_H

#include "NisseMCPConfig.h"

#include "JsonRPC.h"

#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/ParserConfig.h"

#include <istream>
#include <string>
#include <string_view>
#include <utility>

namespace ThorsAnvil::Nisse::MCP
{

/*
 * Reads a Server-Sent Events stream one event at a time.
 *      https://html.spec.whatwg.org/multipage/server-sent-events.html
 *
 * Used as a normal extractor:
 *
 *      SSEInfo<Reply>     event;
 *      while (stream >> event) {
 *          use(event.id, event.event, event.data);
 *      }
 *
 * Deliberate limitations, all detected rather than stumbled over:
 *      - "retry:" is read and discarded. This client does not reconnect.
 *      - A leading byte order mark is not stripped.
 */
class SSEInfo
{
    public:
        std::string     id;         // The "last event ID". Persists until replaced.
        std::string     event;      // Reset to empty for every event.
        std::string     dataBuffer;

        virtual ~SSEInfo()     {}


    private:
        // What the start of a line turned out to be.
        enum class LineKind
        {
            EndOfInput,     // No characters remained.
            Blank,          // An empty line: the event boundary.
            Comment,        // The line began with ':'.
            Field,          // Name read; its value follows at the stream position.
        };

        // Reads one complete event. Sets failbit if none was available.
        std::istream& readEvent(std::istream& stream);

        // Utility functions used by readEvent.
        void consumePairedLF(std::streambuf& buffer);
        void skipLine(std::streambuf& buffer);
        void readLine(std::streambuf& buffer, std::string& dst);
        LineKind readFieldName(std::streambuf& buffer, std::string& name);

        static constexpr int   endOfFile = std::char_traits<char>::eof();

    public:
        SSEInfo() = default;
        explicit SSEInfo(std::string lastEventId)
            : id{std::move(lastEventId)}
        {}

        friend std::istream& operator>>(std::istream& stream, SSEInfo& dst)
        {
            // Build the event separately and only publish a complete one, so a failed
            // read leaves the caller's object untouched. Starting from a fresh object
            // is also what resets "event" and "data" between events; the last event ID
            // is the one field carried forward.
            SSEInfo    next{dst.id};
            if (next.readEvent(stream)) {
                next.swap(dst);
            }
            return stream;
        }

        void swap(SSEInfo& other) noexcept
        {
            using std::swap;
            swap(id,         other.id);
            swap(event,      other.event);
            swap(dataBuffer, other.dataBuffer);
        }
};

inline void swap(SSEInfo& lhs, SSEInfo& rhs) noexcept
{
    lhs.swap(rhs);
}

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "SSEInfo.source"
#endif

#endif
