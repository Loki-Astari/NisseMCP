#include <gtest/gtest.h>

#include "JsonRPC.h"
#include "Context.h"
#include "JsonRPCCore.h"
#include "JsonRPCLocal.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/Traits.h"
#include <numeric>
#include <sstream>


#include "Local.h"

// Examples of expected output from JsonRCP calls.
//    https://www.jsonrpc.org/specification

struct SubtractParam
{
    int     minuend;
    int     subtrahend;
};

using namespace ThorsAnvil::Nisse::MCP;

struct LocalTest: public JsonRPCLocal
{
    std::size_t     size = 0;
    bool            used = false;
    public:
        LocalTest()
        {
            getCore().addExecutor("subtract",     [&](Context& context, JsonRPC::OptRequestId id, SubtractParam const& param){context.addItem(id, param.minuend - param.subtrahend);});
            getCore().addExecutor("update",       [&](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& param){size = param.size();context.addItem(id, 1);});
            getCore().addExecutor("foobar",       [&](Context& context, JsonRPC::OptRequestId id){used = true;context.addItem(id, 1);});

            getCore().addExecutor("sum",          [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& args){context.addItem(id, std::accumulate(std::begin(args), std::end(args), 0));});
            getCore().addExecutor("notify_hello", [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& /*a*/){context.addItem(id, 1);});
            getCore().addExecutor("get_data",     [](Context& context, JsonRPC::OptRequestId id){std::vector<std::string> result; result.emplace_back("hello"); result.emplace_back("5"); context.addItem(id, result);});
            getCore().addExecutor("notify_sum",   [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& args){context.addItem(id, std::accumulate(std::begin(args), std::end(args), 0));});
            getCore().addExecutor("note",         [](Context& context, JsonRPC::OptRequestId id)->int {throw std::runtime_error("Hi");});
            getCore().addExecutor("Hi",           [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const&){context.addItem(id, "Hi");});
            getCore().addExecutor("throw",        [](Context& context, JsonRPC::OptRequestId id, std::vector<int> const& args)->int {throw std::runtime_error("Checking");});
        }
        std::size_t getSize() const {return size;}
        bool        isUsed()  const {return used;}
};

ThorsAnvil_MakeTrait(SubtractParam, minuend, subtrahend);

TEST(JsonLocalRPCProtocolTest, RPC_CallWithPositionalParameters1)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":1})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_UsingAStringID)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": "long-string-that-forms-id"})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":"long-string-that-forms-id"})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_UsingAStructureID)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": {"name": "long-string-that-forms-id"}})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_UsingAnArrayID)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": ["name", "long-string-that-forms-id"]})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_UsingABoolID)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": true})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_UsingANullID)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": null})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_UsingAFloatID)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 22.234})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_CallWithPositionalParameters2)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": [23, 42], "id": 2})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":-19,"id":2})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_CallWithNamedParameters1)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"subtrahend": 23, "minuend": 42}, "id": 3})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":3})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_CallWithNamedParameters2)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "subtract", "params": {"minuend": 42, "subtrahend": 23}, "id": 4})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","result":19,"id":4})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_Notification1)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"()", result.str());
    EXPECT_EQ(5, local.getSize());
}

TEST(JsonLocalRPCProtocolTest, RPC_Notification2)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar"})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"()", result.str());
    EXPECT_TRUE(local.isUsed());
}

TEST(JsonLocalRPCProtocolTest, RPC_NonExistentMethod)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobarbaz", "id": "1"})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"1"})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidJson)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz])"};
                                                                                        //     ^^^^^
                                                                                        // Missing close quote
                                                                                        // Bad close ']' not '}'
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidRequest1)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": 1, "params": "bar"})"};
                                                            //   ^ Invalid Type: Should be string.
    std::ostringstream   result;

    local.run(session, command, result);

    // This is deteted as PARSE Errors. because the method must be a string.
    // EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidRequest2)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.1", "method": "name", "params": "bar"})"};
                                                            //   ^ Invalid Type: Should be string.
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_EmptyBatch)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"([])"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidEmptyBatch)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"([)"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})", result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidRequests1)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"([1,2,3])"};
    std::ostringstream   result;

    local.run(session, command, result);

    /* Specs say this */
#if 0
    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null})"
              R"(])"
                , result.str());
#endif
    // But we check the type of each member on input. This is not a JsonRPC request so we get a parse error.
    // When trying to read the first command.
    // Because we have a parse errors (this is bad JSON so we have to abort any further reading.
    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})"
              R"(])"
                , result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidRequests2)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},)"
                                    R"({"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"},)"
                                    R"({"foo": "boo"},)"
                                    R"({"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},)"
                                    R"({"jsonrpc": "2.0", "method": "get_data", "id": "9"} )"
                                 R"(])"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","result":7,"id":"1"},)"
                R"({"jsonrpc":"2.0","result":19,"id":"2"},)"
                R"({"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null},)"
                R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"5"},)"
                R"({"jsonrpc":"2.0","result":["hello","5"],"id":"9"})"
              R"(])"
                , result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_InvalidRequestsBADJSONInArray)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]},)"
                                    R"({"jsonrpc": "2.0", "method": "subtract", "params": [42,23], "id": "2"})" // Not Missing comma
                                    R"({"foo": "boo"},)"
                                    R"({"jsonrpc": "2.0", "method": "foo.get", "params": {"name": "myself"}, "id": "5"},)"
                                    R"({"jsonrpc": "2.0", "method": "get_data", "id": "9"} )"
                                 R"(])"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"([)"
                R"({"jsonrpc":"2.0","result":7,"id":"1"},)"
                R"({"jsonrpc":"2.0","result":19,"id":"2"},)"
                R"({"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null})"
              R"(])"
                , result.str());
}

TEST(JsonLocalRPCProtocolTest, RPC_BatchNotification)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"([)"
                                    R"({"jsonrpc": "2.0", "method": "notify_sum", "params": [1,2,4]},)"
                                    R"({"jsonrpc": "2.0", "method": "notify_hello", "params": [7]})"
                                 R"(])"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ("", result.str());
}

TEST(JsonLocalRPCProtocolTest, CatchExceptionsOutOfExecutor1)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "throw", "params": [1,2,4], "id": 5})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Internal error"},"id":5})",
            result.str());
}

TEST(JsonLocalRPCProtocolTest, CatchExceptionsOutOfExecutor2)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "note", "id": 5})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32603,"message":"Internal error"},"id":5})",
            result.str());
}

TEST(JsonLocalRPCProtocolTest, CheckForInvalidParameters)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "sum", "params": ["1","2","4"], "id": 5})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32602,"message":"Invalid params"},"id":5})",
            result.str());
}

TEST(JsonLocalRPCProtocolTest, CheckForInvalidParametersPassedToFuncThatTakesZero)
{
    JsonRPCSession       session;
    LocalTest            local;
    std::istringstream   command{R"({"jsonrpc": "2.0", "method": "Hi", "params": 1, "id": 5})"};
    std::ostringstream   result;

    local.run(session, command, result);

    EXPECT_EQ(R"({"jsonrpc":"2.0","error":{"code":-32602,"message":"Invalid params"},"id":5})",
            result.str());
}
