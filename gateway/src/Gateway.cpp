#include "Gateway.hpp"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <string.h>

#include "common/utils/Text.hpp"
#include "common/NodeConfiguration.hpp"


typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

void gatewayConnection::start() { read_request(); }

void gatewayConnection::read_request() {
  std::thread{[q{std::move(socket)}, this]() {
    beast::websocket::stream<tcp::socket> websocket{
        std::move(const_cast<tcp::socket &>(q))};
    websocket.accept();
    while (true) {
      try {
        beast::flat_buffer buffer;
        websocket.read(buffer);
        auto out = beast::buffers_to_string(buffer.cdata());
        this->proccesRequestData(out);
        websocket.write(buffer.data());
      } catch (beast::system_error const &error) {
        if (error.code() == beast::websocket::error::closed) {
          break;
        }
      }
    }
  }}.detach();
}

void gatewayConnection::proccesRequestData(std::string data) const {

  serialization::HeaderSerializer headerSerializer;

  std::vector<std::string> dataAttributes;  
  boost::char_separator<char> at("@");
  tokenizer dataTokens(data, at);

  BOOST_FOREACH (std::string const &token, dataTokens) {
    dataAttributes.push_back(token);
  }

   std::vector<std::string> dataAttributess = common::utils::Text::splitBySeparator(data, "@");

  auto header = dataAttributes[0];
  auto rawMessage = dataAttributes[1];
  auto headerObject = headerSerializer.deserialize(header);

  // std::cout<<"target "<<headerObject.target<<std::endl;
  // std::cout<<"serverName "<<headerObject.serverName<<std::endl;
  // std::cout<<"queName "<<headerObject.queName<<std::endl;
  // std::cout<<"clientAddress "<<headerObject.clientAddress<<std::endl;
  // std::cout<<std::endl;

  if (headerObject.target == "CreateChannel") {
    auto message = headerObject.target + "@" + rawMessage;
    this->SendMessage("amqp://localhost/", headerObject.serverName, headerObject.queName, message);
    std::cout << "CreateChannel request received\n";
  }
  if (headerObject.target == "Send") {
    this->SendMessage("amqp://localhost/", headerObject.serverName, headerObject.queName, rawMessage);
    std::cout << "Send request received\n";
  }
  if (headerObject.target == "GetHistory") {
    std::cout << "GetData request received\n";
  }
}

void gatewayConnection::SendMessage(std::string nodeAddress,
                                       std::string nodeName,
                                       std::string queName,
                                       std::string message) const {

  common::NodeInfo node{nodeAddress, nodeName};
  auto *loop = ev_loop_new(0);
  common::ConnectionHandler handler(loop);

  AMQP::TcpConnection connection(&handler, AMQP::Address(nodeAddress));
  AMQP::TcpChannel channel(&connection);
  channel.onError([](const char *message) {
    std::cout << "Channel error: " << message << std::endl;
  });

  channel.declareQueue(queName, AMQP::durable)
      .onSuccess([&connection, &channel, &node,
                  &message](const std::string &name, uint32_t messagecount,
                            uint32_t consumercount) {
        channel.publish("", name, message);
      })
      .onError([](const char *message) { std::cout << message << "\n"; })
      .onFinalize([&connection]() { connection.close(); });

  
  channel.declareQueue("Client_ControlChannel", AMQP::durable)
      .onSuccess([&connection, &channel, &node,
                  &message](const std::string &name, uint32_t messagecount,
                            uint32_t consumercount) {
      })
      .onError([](const char *message) { std::cout << message << "\n"; });

  channel.consume("Client_ControlChannel", AMQP::noack)
      .onReceived([](const AMQP::Message &msg, uint64_t tag,
                                        bool redelivered) {
        std::cout<<"message consumed "<<msg.body()<<"\nbody size "<<msg.bodySize()<<std::endl;
      });

  ev_run(loop, 0);
}

void gatewayServer(tcp::acceptor &acceptor, tcp::socket &socket) {
  acceptor.async_accept(socket, [&](beast::error_code ec) {
    if (!ec)
      std::make_shared<gatewayConnection>(std::move(socket))->start();
    gatewayServer(acceptor, socket);
  });
}