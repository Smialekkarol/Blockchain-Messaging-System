#include "Client.hpp"

    int Client::MakeInitialConnection(std::string host, char* port, std::string queName)
    {
        try
        {
            std::string message = "CreateChannel;" + nodeName + ";" + queName + "\n";
            int result = this->WebSocketConnection(host, port, message);
        }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return 0;
    }

    int Client::getData(std::string host, char* port, std::string queName, std::string data)
    {
        try
        {
           auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch()).count();
             std::cout<<time<<std::endl;

            std::string message = "GetData;" +std::string(nodeName) + ";" + std::string(queName) + ";" + data + "\n";
            int result = this->WebSocketConnection(host, port, message);
        }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return 0;
    }

    int Client::SendMessage(std::string host, char* port, std::string queName, std::string data)
    {
        try
        {
            std::string message = "Send;" +std::string(nodeName) + ";" + std::string(queName) + ";" + data + "\n";
            int result = this->WebSocketConnection(host, port, message);
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
                        req.target("Create");
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