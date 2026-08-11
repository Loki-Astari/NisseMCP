#include <gtest/gtest.h>
#include "MCPServer.h"
#include "ClientMCP.h"
#include "CommandPing.h"
#include "CommandLogging.h"
#include "ThorsLogging/loguru.hpp"

using namespace ThorsAnvil::Nisse::MCP;

namespace ThorsAnvil::Nisse::MCP::UnitTest
{

bool didItLog = false;
NISSEMCP_HEADER_ONLY_INCLUDE
void mcpLogUnitTest(void* userData, const loguru::Message& message)
{
    using ThorsAnvil::Nisse::MCP::MCPSession;
    MCPSession& session = *reinterpret_cast<MCPSession*>(userData);
    if (message.verbosity == 2) // Notice => USed by MCPCore to indicate function call.
    {
        // Here we are checking if the notice action was called for the unit test.
        didItLog = session.log(message);
    }
    else {
        // All other logging is just done normally we dont' care.
        session.log(message);
    }
}

}

namespace
{

struct MCPServerTest: public MCPServer
{
    public:
        MCPServerTest(MCPServerConfig&& config = {.serverName = "Command Set Logging Server 1.0", .allowedOrigin = "localhost", .slot = "/mcp", .protocolInfo = {Protocol::v2025_11_25, Protocol::v2025_11_25}})
            : MCPServer{std::move(config)}
        {}
};

using MCPServerRunner = ThorsAnvil::Nisse::Server::UnitTest::ServerRunner<MCPServerTest>;

}

TEST(ClientLoggingTest, TwoSessionsNoLogging)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
}

TEST(ClientLoggingTest, TwoSessionsOneLogging)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};

    bool ok = false;
    bool error = false;
    int  oldCount = loguru::unitTestInfo();
    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::debug},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);

    EXPECT_EQ(oldCount + 1, loguru::unitTestInfo());
}

TEST(ClientLoggingTest, TwoSessionsOneLoggingThenTurnItOff)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};

    bool ok = false;
    bool error = false;
    int  oldCount = loguru::unitTestInfo();
    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::debug},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 1, loguru::unitTestInfo());

    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::emergency},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );

    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount, loguru::unitTestInfo());
}

TEST(ClientLoggingTest, TwoSessionsOneLoggingThenPing)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};

    bool ok = false;
    bool error = false;
    int  oldCount = loguru::unitTestInfo();
    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::info},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 1, loguru::unitTestInfo());


    UnitTest::didItLog = false;
    connect1.sendRequest<Command::Ping>([&](Command::Object&& /*param*/){ok = true;},
                                        [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                       );
    EXPECT_TRUE(UnitTest::didItLog);
}

TEST(ClientLoggingTest, TwoSessionsOneLoggingThenPingOtherClinet)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};

    bool ok = false;
    bool error = false;
    int  oldCount = loguru::unitTestInfo();
    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::info},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 1, loguru::unitTestInfo());

    UnitTest::didItLog = false;
    connect2.sendRequest<Command::Ping>([&](Command::Object&& /*param*/){ok = true;},
                                        [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                       );
    EXPECT_FALSE(UnitTest::didItLog);
}

TEST(ClientLoggingTest, TwoSessionsOneErrorOneInfoPingError)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};

    bool ok = false;
    bool error = false;
    int  oldCount = loguru::unitTestInfo();
    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::error},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 1, loguru::unitTestInfo());

    ok = false;
    connect2.sendRequest<Command::Logging>({Command::LoggingLevel::info},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 2, loguru::unitTestInfo());

    UnitTest::didItLog = false;
    ok = false;
    connect1.sendRequest<Command::Ping>([&](Command::Object&& /*param*/){ok = true;},
                                        [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                       );
    // No logging because check is on Notice and this only logs errors.
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_FALSE(UnitTest::didItLog);
}

TEST(ClientLoggingTest, TwoSessionsOneErrorOneInfoPingInfo)
{
    MCPServerRunner     server;

    ClientMCP           connect1{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};
    ClientMCP           connect2{{"/mcp", Protocol::v2025_11_25}, {"localhost", 8070}};

    bool ok = false;
    bool error = false;
    int  oldCount = loguru::unitTestInfo();
    connect1.sendRequest<Command::Logging>({Command::LoggingLevel::error},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 1, loguru::unitTestInfo());

    ok = false;
    connect2.sendRequest<Command::Logging>({Command::LoggingLevel::info},
                                            [&](Command::Object&& /*param*/){ok = true;},
                                            [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                          );
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_EQ(oldCount + 2, loguru::unitTestInfo());

    UnitTest::didItLog = false;
    ok = false;
    connect2.sendRequest<Command::Ping>([&](Command::Object&& /*param*/){ok = true;},
                                        [&](int /*s*/, int /*code*/, std::string_view /*message*/){error = true;}
                                       );
    // No logging because check is on Notice and this only logs errors.
    EXPECT_TRUE(ok);
    EXPECT_FALSE(error);
    EXPECT_TRUE(UnitTest::didItLog);
}

