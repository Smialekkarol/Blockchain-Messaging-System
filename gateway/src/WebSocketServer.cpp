#include "WebSocketServer.hpp"


void websocket_connection::start()
    {
        read_request();
    }

void websocket_connection::read_request()
  {
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
                            std::cout<<error.code().message()<<std::endl;
                            break;
                        }
                    }
                }

            }}.detach();
  }

void websocket_server(tcp::acceptor & acceptor, tcp::socket& socket){
     acceptor.async_accept(socket,
      [&](beast::error_code ec)
      {
          if(!ec)
              std::make_shared<websocket_connection>(std::move(socket))->start();
          websocket_server(acceptor, socket);
      });
}