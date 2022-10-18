#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

int main()
{
    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned>(std::stoi("8083"));
    boost::asio::io_context ioc{1};
    
    boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};

    while(1){
        tcp::socket socket{ioc};
        acceptor.accept(socket);
        std::cout<<"socket accepted\n";

        std::thread{
            [q{std::move(socket)}](){
                
                beast::websocket::stream<tcp::socket> websocket{std::move(const_cast<tcp::socket&>(q))};
                websocket.accept();
                while(1)
                {
                    try{
                    beast::flat_buffer buffer;
                    websocket.read(buffer);

                    auto out = beast::buffers_to_string(buffer.cdata());
                    std::cout<<out<<std::endl;
                    websocket.write(buffer.data());
                    }
                    catch( beast::system_error const & error)
                    {
                        if(error.code() == beast::websocket::error::closed)
                        {
                            break;
                            std::cout<<error.what();
                        }
                    }
                }

            }}.detach();
    }
   
}