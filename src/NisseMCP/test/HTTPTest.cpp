#include <gtest/gtest.h>

#include "NisseHTTP/Util.h"
#include "NisseHTTP/NisseHTTPServer.h"
#include "NisseHTTP/HTTPHandler.h"
#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"
//#include "NisseHTTP/ClientRequest.h"
#include "NisseHTTP/ClientHTTP.h"
//#include "NisseHTTP/ClientResponse.h"
#include "ThorsSocket/SocketStream.h"
#include "ThorSerialize/JsonThor.h"

class ServerRunner: public ThorsAnvil::Nisse::HTTP::NisseHTTPServer
{
    public:
        ServerRunner()
            : NisseHTTPServer{1, ThorsAnvil::ThorsSocket::ServerInfo{8080}, ThorsAnvil::ThorsSocket::ServerInfo{8070}}
        {

            addPath(ThorsAnvil::Nisse::HTTP::Method::GET, "/mcp", [](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response)
            {
                response.body(ThorsAnvil::Nisse::HTTP::Encoding::Chunked) << "\"Morning\"";
                return true;    // Indicates we handeled the request. Don't search for more matches.
            });
        }
};

class LocalServer
{
    std::thread     thread;
    public:
        LocalServer()
            : thread{[]()
              {
                    ServerRunner                server;
                    server.run();
              }}
        {}
        ~LocalServer()
        {
            ThorsAnvil::Nisse::HTTP::ClientHTTP    client({"127.0.0.1", 8070}, ThorsAnvil::Nisse::HTTP::Version::HTTP1_0);
            client.get<std::string>({.path = "/?command=stophard"});
            thread.join();
        }
};

// ThorsAnvil::Serialize::PrinterConfig    ServerRunner::outputConfig{ThorsAnvil::Serialize::OutputType::Stream};

TEST(HTTPTest, ServerRun)
{
    LocalServer     server;

    ThorsAnvil::Nisse::HTTP::ClientHTTP client{ThorsAnvil::ThorsSocket::SocketInfo{"localhost", 8080}};
    std::string reply = client.get<std::string>({.path = "/mcp"});

    EXPECT_EQ("Morning", reply);

#if 0
                EXPECT_EQ(resp.getStatus(), 200);
                EXPECT_EQ(resp.getMessage(), "OK");
                ASSERT_TRUE(resp.getHeader().hasHeader("transfer-encoding"sv));
                EXPECT_EQ(resp.getHeader().getHeader("transfer-encoding")[0], "chunked");
    using namespace std::string_literals;
    std::string line = runner.sendMessage("A message"s);
    std::cerr << "Reply:<\n" << line << "\n=======\n";
    EXPECT_EQ("Morning", line);
#endif
}
