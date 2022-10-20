#include "HTTPServer.hpp"

    void http_connection::start()
    {
        read_request();
    }


    void http_connection::read_request()
    {
        auto self = shared_from_this();

        http::async_read(
            socket_,
            buffer_,
            request_,
            [self](beast::error_code ec,
                std::size_t bytes_transferred)
            {
                boost::ignore_unused(bytes_transferred);
                if(!ec)
                    self->process_request();
            });
    }

    void http_connection::process_request()
    {
        response_.version(request_.version());
        response_.keep_alive(false);

        switch(request_.method())
        {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_response();
            break;

        case http::verb::post:
            response_.result(http::status::ok);
            response_.set(http::field::server, "text/plain");
             beast::ostream(response_.body())
                << request_.body();
            std::cout<<request_.body();
            process_request_data(request_.body());
            break;

        default:
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body())
                << "Invalid request-method '"
                << std::string(request_.method_string())
                << "'";
            break;
        }

        write_response();
    }

    void http_connection::process_request_data(std::string name){
        const std::string &message{"test message\n"};
        std::string ip{"amqp://localhost/"};
        NodeInfo node{ip, name};
        auto *loop = ev_loop_new(0);
        ConnectionHandler handler(loop);

        AMQP::TcpConnection connection(&handler, AMQP::Address(ip));
        AMQP::TcpChannel channel(&connection);
        channel.onError([](const char *message) {
            std::cout << "Channel error: " << message << std::endl;
        });
        channel.declareQueue(name, AMQP::durable)
            .onSuccess([&connection, &channel, &node,
                        &message](const std::string &name, uint32_t messagecount,
                                    uint32_t consumercount) {
                channel.publish("", name, message);
            })
            .onError([](const char *message) { std::cout << message << "\n"; })
            .onFinalize([&connection]() { connection.close(); });
        ev_run(loop, 0);
    }

    void http_connection::create_response()
    {
        if(request_.target() == "")
        {
            response_.set(http::field::content_type, "text/html");
            beast::ostream(response_.body())
                << "";
        }
        else
        {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "File not found\r\n";
        }
    }

    void http_connection::write_response()
    {
        auto self = shared_from_this();

        response_.content_length(response_.body().size());

        http::async_write(
            socket_,
            response_,
            [self](beast::error_code ec, std::size_t)
            {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            });
    }


void http_server(tcp::acceptor& acceptor, tcp::socket& socket)
{
  acceptor.async_accept(socket,
      [&](beast::error_code ec)
      {
          if(!ec)
              std::make_shared<http_connection>(std::move(socket))->start();
          http_server(acceptor, socket);
      });
}
