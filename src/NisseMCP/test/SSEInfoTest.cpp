#include <gtest/gtest.h>
#include "JsonRPC.h"
#include "SSEInfo.h"
#include "ThorSerialize/Traits.h"
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// https://html.spec.whatwg.org/multipage/server-sent-events.html
//
// Note: the tests below spell out "\n" / "\r\n" explicitly rather than using raw
// string literals. Line endings and leading spaces are semantically significant to
// a line oriented parser, so it is worth being able to see exactly what is on the
// wire without counting whitespace in the source.

// SSEInfo reports a payload that fails to parse by setting "error" on the
// DataType, so every DataType needs one.
struct TestData
{
    ThorsAnvil::Nisse::MCP::JsonRPC::OptError            error;
};
ThorsAnvil_MakeTrait(TestData, error);

// Payload with a field, so a test can prove that the correct bytes reached the
// JSON parser rather than simply that an event was dispatched.
struct Payload
{
    int     n = 0;
    ThorsAnvil::Nisse::MCP::JsonRPC::OptError            error;
};
ThorsAnvil_MakeTrait(Payload, n, error);

using SSETestEvet   = ThorsAnvil::Nisse::MCP::SSEInfo<TestData>;
using SSEPayload    = ThorsAnvil::Nisse::MCP::SSEInfo<Payload>;

// A snapshot of one dispatched event, so a whole stream can be asserted on at once.
struct Captured
{
    std::string     id;
    std::string     event;
    int             n;
    int             errorCode;
};

std::vector<Captured> readAll(std::istream& input)
{
    std::vector<Captured>  result;
    SSEPayload             event;
    while (input >> event) {
        result.push_back(Captured{event.id, event.event, event.data.n, event.data.error.has_value() ? event.data.error.value().code : 0});
    }
    return result;
}

// ---------------------------------------------------------------------------
// Well formed events
// ---------------------------------------------------------------------------

TEST(SSEInforTest, Data)
{
    std::stringstream input{
R"(data: {}

)"};

    std::size_t     count = 0;
    SSETestEvet     event;
    while (input >> event) {
        ++count;
    }

    ASSERT_EQ(1, count);
}

TEST(SSEInfoTest, DataValueReachesParser)
{
    std::stringstream   input{"data: {\"n\":42}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(42, events[0].n);
}

TEST(SSEInforTest, EventTypeAndData)
{
    std::stringstream   input{"event: message\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ("message", events[0].event);
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, IdEventAndData)
{
    std::stringstream   input{"id: 17\nevent: message\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ("17", events[0].id);
    EXPECT_EQ("message", events[0].event);
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, ThreeEventsInSequence)
{
    std::stringstream   input{"data: {\"n\":1}\n\ndata: {\"n\":2}\n\ndata: {\"n\":3}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(3, events.size());
    EXPECT_EQ(1, events[0].n);
    EXPECT_EQ(2, events[1].n);
    EXPECT_EQ(3, events[2].n);
}

TEST(SSEInfoTest, NoSpaceAfterColon)
{
    std::stringstream   input{"data:{\"n\":7}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(7, events[0].n);
}

TEST(SSEInfoTest, OnlyOneLeadingSpaceIsStripped)
{
    std::stringstream   input{"event:  message\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(" message", events[0].event);
}

// ---------------------------------------------------------------------------
// Line endings
// ---------------------------------------------------------------------------

TEST(SSEInfoTest, CarriageReturnLineFeed)
{
    std::stringstream   input{"event: message\r\ndata: {\"n\":1}\r\nid: 5\r\n\r\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ("message", events[0].event);
    EXPECT_EQ("5", events[0].id);
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, BareCarriageReturn)
{
    std::stringstream   input{"event: message\rdata: {\"n\":1}\r\r"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ("message", events[0].event);
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, MixedLineEndings)
{
    std::stringstream   input{"event: message\r\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ("message", events[0].event);
    EXPECT_EQ(1, events[0].n);
}

// ---------------------------------------------------------------------------
// Comments, blank lines, keep alive
// ---------------------------------------------------------------------------

TEST(SSEInforTest, CommentBeforeEventIsIgnored)
{
    std::stringstream   input{": this is a comment\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
}

// The common server keep alive: a bare ":ping" between real events.
TEST(SSEInfoTest, KeepAlivePingBetweenEvents)
{
    std::stringstream   input{":ping\ndata: {\"n\":1}\n\n:ping\n\ndata: {\"n\":2}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(2, events.size());
    EXPECT_EQ(1, events[0].n);
    EXPECT_EQ(2, events[1].n);
}

TEST(SSEInfoTest, StreamOfOnlyCommentsProducesNoEvents)
{
    std::stringstream   input{": one\n: two\n\n"};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

TEST(SSEInfoTest, LeadingBlankLinesAreIgnored)
{
    std::stringstream   input{"\n\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, TrailingBlankLinesAfterFinalEvent)
{
    std::stringstream   input{"data: {\"n\":1}\n\n\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, EventWithoutDataIsNotDispatched)
{
    std::stringstream   input{"event: alpha\n\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ("", events[0].event);
    EXPECT_EQ(1, events[0].n);
}

// ---------------------------------------------------------------------------
// Field persistence
// ---------------------------------------------------------------------------

TEST(SSEInfoTest, IdPersistsToFollowingEvent)
{
    std::stringstream   input{"id: abc\ndata: {\"n\":1}\n\ndata: {\"n\":2}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(2, events.size());
    EXPECT_EQ("abc", events[0].id);
    EXPECT_EQ("abc", events[1].id);
}

TEST(SSEInforTest, IdReplacedByLaterEvent)
{
    std::stringstream   input{"id: a\ndata: {\"n\":1}\n\nid: b\ndata: {\"n\":2}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(2, events.size());
    EXPECT_EQ("a", events[0].id);
    EXPECT_EQ("b", events[1].id);
}

// Unlike the id, the event type resets to empty for every event.
TEST(SSEInfoTest, EventTypeDoesNotPersist)
{
    std::stringstream   input{"event: alpha\ndata: {\"n\":1}\n\ndata: {\"n\":2}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(2, events.size());
    EXPECT_EQ("alpha", events[0].event);
    EXPECT_EQ("", events[1].event);
}

// ---------------------------------------------------------------------------
// Fields that must be skipped
// ---------------------------------------------------------------------------

TEST(SSEInfoTest, RetryFieldIsIgnored)
{
    std::stringstream   input{"retry: 3000\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
}

TEST(SSEInfoTest, UnknownFieldIsIgnored)
{
    std::stringstream   input{"foo: bar\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
}

// Shares its first letter with "data", so it exercises the mid-name mismatch path
// in checkFor(). The mismatch happens before the line ending, so ignoreLine()
// correctly finishes the current line.
TEST(SSEInfoTest, UnknownFieldSharingAPrefixIsIgnored)
{
    std::stringstream   input{"duration: 5\ndata: {\"n\":42}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(42, events[0].n);
}

// ---------------------------------------------------------------------------
// Stream termination
// ---------------------------------------------------------------------------

TEST(SSEInforTest, EmptyStreamProducesNoEvents)
{
    std::stringstream   input{""};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

// "Once the end of the file is reached, any pending data must be discarded. (If the
// file ends in the middle of an event, before the final empty line, the incomplete
// event is not dispatched.)" An event with no terminating blank line is therefore
// dropped rather than delivered.
//
// Note this currently falls out of std::istream::get() setting failbit as well as
// eofbit when it cannot extract a character, which makes readEventFromStream()
// return a failed stream and suppresses the swap in operator>>. The explicit
// "wasThereData" branch in the eof case reads as though it intended to return
// success here. Take care that any fix for the badbit issue below does not start
// dispatching incomplete events.
TEST(SSEInforTest, IncompleteFinalEventIsDiscarded)
{
    std::stringstream   input{"data: {\"n\":1}\n"};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

TEST(SSEInforTest, IncompleteFinalEventWithNoNewlineIsDiscarded)
{
    std::stringstream   input{"data: {\"n\":1}"};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

// ---------------------------------------------------------------------------
// Known issues.
//
// These describe the behaviour the parser should have. They are expected to fail
// against the parser as it currently stands, and are kept in a separate suite so
// that the passing suite above can be run on its own:
//
//     make test-SSEInforTest.*
//     make test-SSEInforKnownIssueTest.*
// ---------------------------------------------------------------------------

// Reaching the end of a well formed stream is not an error. Today the final read
// sets badbit, and ClientMCP::handleStreamRespFromServer tests body().fail() -
// which is (failbit || badbit) - so every successful stream reports an error.
TEST(SSEInforKnownIssueTest, CleanEndOfStreamIsNotAnError)
{
    std::stringstream   input{"data: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_FALSE(input.bad());
}

// A field name with no colon is legal and means "empty value". checkFor() consumes
// the mismatching character without ungetting it, so when the mismatch is the line
// ending itself the caller's ignoreLine() swallows the whole of the next line.
TEST(SSEInfoTest, FieldNameWithNoColonDoesNotEatTheNextLine)
{
    std::stringstream   input{"id\ndata: {\"n\":42}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(42, events[0].n);
    EXPECT_EQ("", events[0].id);
}

TEST(SSEInfoTest, UnknownFieldStartingWithDDoesNotDispatch)
{
    std::stringstream   input{"dodgy: value\n\n"};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

// ---------------------------------------------------------------------------
// Termination and the stream state contract
// ---------------------------------------------------------------------------

TEST(SSEInfoTest, EmptyStreamProducesNoEvents)
{
    std::stringstream   input{""};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
    EXPECT_FALSE(input.bad());
}

TEST(SSEInfoTest, IncompleteFinalEventIsDiscarded)
{
    std::stringstream   input{"data: {\"n\":1}\n"};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

TEST(SSEInfoTest, IncompleteFinalEventWithNoNewlineIsDiscarded)
{
    std::stringstream   input{"data: {\"n\":1}"};
    auto                events = readAll(input);

    EXPECT_EQ(0, events.size());
}

// Reaching the end of the input is not an error: eof set, bad clear.
TEST(SSEInfoTest, CleanEndOfStreamIsNotAnError)
{
    std::stringstream   input{"data: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_TRUE(input.eof());
    EXPECT_FALSE(input.bad());
}

// A malformed payload is converted into an error object on the stream of returned objects.
TEST(SSEInfoTest, MalformedPayloadIsNotReportedAsEndOfInput)
{
    std::stringstream   input{"data: not json at all\n\ndata: {\"n\":1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(2, events.size());
    EXPECT_EQ(-32700, events[0].errorCode);
    EXPECT_EQ(1, events[1].n);
    EXPECT_FALSE(input.bad());
    EXPECT_TRUE(input.fail());
    EXPECT_TRUE(input.eof());
}

// A second "data:" field is caught
// The two data fields are concatenated and parsed together.
// This code expects exactly one JSON object in the data and thus 2 objects
// will result in an error object being created instead.
TEST(SSEInfoTest, RepeatedDataFieldIsRejectedCleanly)
{
    std::stringstream   input{"data: {\"n\":1}\ndata: {\"n\":2}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(-32700, events[0].errorCode);
    EXPECT_TRUE(input.fail());
    EXPECT_TRUE(input.eof());
    EXPECT_FALSE(input.bad());
}

// The spec concatenates repeated data fields with "\n" and parses the result, which
// lets a server pretty print its JSON across several lines. readFieldDataValue()
// parses straight off the stream, so the first partial line fails to parse.
TEST(SSEInforKnownIssueTest, MultipleDataLinesAreConcatenated)
{
    std::stringstream   input{"data: {\"n\":\ndata: 1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
    EXPECT_TRUE(input.fail());
    EXPECT_TRUE(input.eof());
    EXPECT_FALSE(input.bad());
}
