#include <thread>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>

#include "GatewayAsync.hpp"
#include "common/ConnectionHandler.hpp"
#include "common/utils/Text.hpp"
#include "common/utils/Timer.hpp"

namespace gateway {

inline void fail(beast::error_code ec, char const *what) 
{
  std::cerr << what << ": " << ec.message() << "\n";
}

Session::Session(tcp::socket &&socket)
    : websocket(std::move(socket)), loop(ev_loop_new(0)) {}

void Session::run() 
{
  net::dispatch(websocket.get_executor(),
                beast::bind_front_handler(&Session::onRun, shared_from_this()));
}

void Session::onRun() 
{
  websocket.set_option(
      websocket::stream_base::decorator([](websocket::response_type &res) {
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) +
                                         " websocket-server-async");
      }));
  websocket.async_accept(
      beast::bind_front_handler(&Session::onAccept, shared_from_this()));
}

void Session::onAccept(beast::error_code ec) 
{
  if (ec)
    return fail(ec, "accept");
  doRead();
}

void Session::doRead() 
{
  buffer.shrink_to_fit();
  websocket.async_read(
      buffer, beast::bind_front_handler(&Session::onRead, shared_from_this()));
}

void Session::onRead(beast::error_code ec, std::size_t bytes_transferred) 
{
  boost::ignore_unused(bytes_transferred);
  if (ec == websocket::error::closed)
    return;

  if (ec)
    return fail(ec, "read");

  websocket.text(websocket.got_text());

  // here we can access the data

  std::string bufferedData = beast::buffers_to_string(buffer.cdata());
  proccesRequestData(bufferedData);
  auto callable = [&]() {
                std::string serializedData;
                if (messageBuffer.getSize() > 0) {
                    for (auto msg : messageBuffer.getBufferedData()) {
                        serializedData += msg + "@";
                    }
                    std::cout<<"before write\n";
                    messageBuffer.clearBuffer();
                    websocket.write(net::buffer(std::string(serializedData)));
                    std::cout<<"after write\n";
                    
                }
  };
  Timer<std::function<void()>> timer;
timer.setInterval(callable, 2000);

  buffer.consume(buffer.size());
  doRead();
}

void Session::onWrite(beast::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return fail(ec, "write");
  buffer.consume(buffer.size());
  doRead();
}

void Session::proccesRequestData(std::string &data) {
  serialization::HeaderSerializer headerSerializer;
  std::vector<std::string> dataAttributes =
      common::utils::Text::splitBySeparator(data, "@");

  auto header = dataAttributes[0];
  auto rawMessage = dataAttributes[1];
  auto headerObject = headerSerializer.deserialize(header);

  if (headerObject.target == "CreateChannel") {
    auto message = headerObject.target + "@" + rawMessage;
    this->SendMessageToNode("amqp://localhost/", headerObject, message);
    std::cout<<"create\n";
  }
  if (headerObject.target == "Send") {
    this->SendMessageToNode("amqp://localhost/", headerObject, rawMessage);
    std::cout<<"send\n";
  }
  if (headerObject.target == "GetHistory") {}
}

void Session::SendMessageToNode(std::string nodeAddress,
                                common::itf::Header header,
                                std::string message) {

  common::NodeInfo node{header.serverName, nodeAddress};
  common::ConnectionHandler handler(loop);

  AMQP::TcpConnection connection(&handler, AMQP::Address(nodeAddress));
  AMQP::TcpChannel channel(&connection);

  serialization::MessageSerializer messageSerializer;

  channel.onError([](const char *message) {
    std::cout << "Channel error: " << message << std::endl;
  });

  channel.declareQueue(header.queName, AMQP::durable)
      .onSuccess([&connection, &channel, &node, &message](
          const std::string &name, uint32_t messagecount,
          uint32_t consumercount) { 
            
            channel.publish("", name, message); 
            })
      .onError([](const char *message) { std::cout << message << "\n"; })
      .onFinalize([&connection]() { connection.close(); });

  std::string clientControlChannelName =
      header.clientAddress + "_ClientChannel";

  channel.declareQueue(clientControlChannelName, AMQP::durable)
      .onSuccess([&connection, &channel, &node,
                  &message](const std::string &name, uint32_t messagecount,
                            uint32_t consumercount) {})
      .onError([](const char *message) { std::cout << message << "\n"; });


     channel.consume(clientControlChannelName, AMQP::noack)
      .onReceived([&messageSerializer, this](const AMQP::Message &msg, uint64_t tag,
                                       bool redelivered) {

          std::vector<std::string> data =
            common::utils::Text::splitBySeparator(msg.body(), "@");
            std::string dataToSend;
        for (auto x = 0; x < data.size() - 1; x++) {
          common::itf::Message messageObject = messageSerializer.deserialize(data[x]);
            messageBuffer.addMessage(data[x]);
            dataToSend += data[x] + "@";
            
            std::cout << messageSerializer.deserialize(data[x]).author<<":"<<messageSerializer.deserialize(data[x]).data << std::endl;
        }
        std::cout<<"message sent to client "<<dataToSend<<std::endl;
        // websocket.write(net::buffer(std::string(dataToSend)));

        });

  ev_run(loop, 0);
}
}