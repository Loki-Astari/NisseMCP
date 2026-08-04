#ifndef THORSANVIL_NISSE_SSEINFO_H
#define THORSANVIL_NISSE_SSEINFO_H

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
class SSEInfoBase
{
    public:
        std::string     id;         // The "last event ID". Persists until replaced.
        std::string     event;      // Reset to empty for every event.

        virtual ~SSEInfoBase()     {}

    protected:
        SSEInfoBase() = default;
        explicit SSEInfoBase(std::string lastEventId)
            : id{std::move(lastEventId)}
        {}

        // Reads one complete event. Sets failbit if none was available.
        std::istream& readEvent(std::istream& stream);

    private:
        // What the start of a line turned out to be.
        enum class LineKind
        {
            EndOfInput,     // No characters remained.
            Blank,          // An empty line: the event boundary.
            Comment,        // The line began with ':'.
            Field,          // Name read; its value follows at the stream position.
        };

        // converts the 'data' field of an SSE event into appropriate data value.
        // This is done in the derived type as we don't know the type in the base class.
        virtual bool readData(std::string_view stream) = 0;


        void consumePairedLF(std::streambuf& buffer);
        void skipLine(std::streambuf& buffer);
        void readLine(std::streambuf& buffer, std::string& dst);
        LineKind readFieldName(std::streambuf& buffer, std::string& name);

        static constexpr int   endOfFile = std::char_traits<char>::eof();


};

template<typename DataType>
class SSEInfo: public SSEInfoBase
{
    public:
        DataType        data;

        SSEInfo() = default;
        explicit SSEInfo(std::string lastEventId)
            : SSEInfoBase{std::move(lastEventId)}
        {}

        void swap(SSEInfo& other) noexcept
        {
            using std::swap;
            swap(id,    other.id);
            swap(event, other.event);
            swap(data,  other.data);
        }

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

    private:
        virtual bool readData(std::string_view stream) override
        {
            static const ThorsAnvil::Serialize::ParserConfig     config{ThorsAnvil::Serialize::ParserConfig{}.setValidateNoTrailingData()};
            if (stream >> ThorsAnvil::Serialize::jsonImporter(data, config)) {
                return true;
            }
            // Failed parse.
            // Should reset any data so that the error state is not polluted with partial data.
            data = DataType{.error = JsonRPC::Error{.code = -32700, .message = "Parse Error"}};

            // Returning true as we have compensated for the error.
            // This will allow the stream to continue being read.
            return true;
        }
};

template<typename DataType>
inline void swap(SSEInfo<DataType>& lhs, SSEInfo<DataType>& rhs) noexcept
{
    lhs.swap(rhs);
}

}

#if defined(NISSEMCP_HEADER_ONLY) && NISSEMCP_HEADER_ONLY == 1
#include "SSEInfo.source"
#endif

#endif
