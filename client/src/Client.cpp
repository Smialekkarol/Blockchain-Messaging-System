#include "Client.hpp"

    int Client::HTTPConnection(std::string host, char* port, std::string message){
        try
        {
            message += "\n";
            auto const target = "/post";
            int version = 11;
            auto const results = resolver.resolve(host, port);
            httpStream.connect(results);

            http::request<http::string_body> req{http::verb::post, target, version};
            req.set(http::field::host, host);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.body() =  message;
            req.prepare_payload();

            http::write(httpStream, req);
            beast::flat_buffer buffer;
            http::response<http::dynamic_body> res;
            http::read(httpStream, buffer, res);
            std::cout<<res<<std::endl;
            beast::error_code ec;
            httpStream.socket().shutdown(tcp::socket::shutdown_both, ec);
            if(ec && ec != beast::errc::not_connected)
                throw beast::system_error{ec};
            }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return 0;
    }

    int Client::WebSocketConnection(std::string host, char* port, std::string message){
        try{
            auto const results = resolver.resolve(host, port);
            auto ep = net::connect(webSocketStream.next_layer(), results);
            host += ':' + std::to_string(ep.port());

            webSocketStream.set_option(websocket::stream_base::decorator(
                [](websocket::request_type& req)
                {
                    req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-coro");
                }));

            webSocketStream.handshake(host, "/");
            webSocketStream.write(net::buffer(std::string(message)));
            beast::flat_buffer buffer;
            webSocketStream.read(buffer);
            webSocketStream.close(websocket::close_code::normal);
            std::cout << beast::make_printable(buffer.data()) << std::endl;
    }
    catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    return 0;
    }