#include <gtest/gtest.h>
#include "JsonRPC.h"
#include "SSEInfo.h"
#include "ThorSerialize/Traits.h"
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// Distinct names: every test .cpp is linked into one binary alongside SSEInfoTest.cpp.
struct Payload
{
    int     n = 0;
    ThorsAnvil::Nisse::MCP::JsonRPC::OptError            error;
};
ThorsAnvil_MakeTrait(Payload, n, error);

using SSEEvent = ThorsAnvil::Nisse::MCP::SSEInfo<Payload>;

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
    SSEEvent               event;
    while (input >> event) {
        result.push_back(Captured{event.id, event.event, event.data.n, event.data.error.has_value() ? event.data.error.value().code : 0});
    }
    return result;
}

// ---------------------------------------------------------------------------
// Well formed events
// ---------------------------------------------------------------------------

TEST(SSEInfoTest, DataValueReachesParser)
{
    std::stringstream   input{"data: {\"n\":42}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(42, events[0].n);
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

TEST(SSEInfoTest, UnknownFieldSharingAPrefixIsIgnored)
{
    std::stringstream   input{"duration: 5\ndata: {\"n\":42}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(42, events[0].n);
}

// A colon-less field is a legal empty value. Reading the name whole before
// dispatching means the following line cannot be swallowed.
TEST(SSEInfoTest, FieldNameWithNoColonDoesNotEatTheNextLine)
{
    std::stringstream   input{"id\ndata: {\"n\":42}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(42, events[0].n);
    EXPECT_EQ("", events[0].id);
}

// A line merely starting with 'd' must not mark the event as carrying data.
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

// JSON pretty printed across data lines splits mid-token,
TEST(SSEInfoTest, PrettyPrintedJsonAcrossDataLinesShouldWork)
{
    std::stringstream   input{"data: {\"n\":\ndata: 1}\n\n"};
    auto                events = readAll(input);

    ASSERT_EQ(1, events.size());
    EXPECT_EQ(1, events[0].n);
    EXPECT_TRUE(input.fail());
    EXPECT_TRUE(input.eof());
    EXPECT_FALSE(input.bad());
}
