#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
namespace ip = boost::asio::ip;
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>



std::string getAddress()
{
    boost::asio::io_service ioService;
    ip::tcp::resolver resolver(ioService);
    return resolver.resolve(ip::host_name(), "")->endpoint().address().to_string();
}

int main(int argc, char** argv)
{
    // std::cout<<getAddress()<<std::endl;
  try{
        if(argc != 4)
        {
            std::cerr <<
                "Usage: HTTPClient <host> <port> <nodeName>\n" <<
                "Example:\n" <<
                "    HTTPClient 127.0.0.1 8080 node1\n";

            return EXIT_FAILURE;
        }
        auto const host = argv[1];
        auto const port = argv[2];
        std::string nodeName = argv[3];
        auto const target = "/post";
        int version = 11;
        nodeName += "\n";


        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

 
        auto const results = resolver.resolve(host, port);
        stream.connect(results);


        http::request<http::string_body> req{http::verb::post, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.body() =  nodeName;
        req.prepare_payload();
    
        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;

        http::read(stream, buffer, res);

        std::cout<<res<<std::endl;

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        if(ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//]