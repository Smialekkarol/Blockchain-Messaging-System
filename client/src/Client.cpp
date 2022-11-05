#include "Client.hpp"

int Client::MakeInitialConnection(std::string host, char *port,
                                  std::string queToBeCreated) {
  try {
    std::string queName = nodeName + "_ControlChannel";

    common::itf::Header header("CreateChannel", nodeName, queName);
    common::itf::Message message(common::utils::Timestamp::get(),
                                 queToBeCreated, clientName);

    serialization::MessageSerializer messageSerializer;
    serialization::HeaderSerializer headerSerializer;

    auto data = headerSerializer.serialize(header) + "@" +
    messageSerializer.serialize(message) + "\n";

    return this->WebSocketConnection(host, port, data.c_str());

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

int Client::SendMessage(std::string host, char *port, std::string queName,
                        std::string dataToSend) {
  try {
    serialization::MessageSerializer messageSerializer;
    serialization::HeaderSerializer headerSerializer;

    common::itf::Header header("Send", nodeName, queName);
    common::itf::Message message(common::utils::Timestamp::get(), dataToSend,
                                 clientName);

    std::string data = headerSerializer.serialize(header) + "@" +
    messageSerializer.serialize(message) + "\n";

    return this->WebSocketConnection(host, port, data);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

int Client::getData(std::string host, char *port, std::string queName,
                    std::string RequestData) {
  try {
    serialization::MessageSerializer messageSerializer;
    serialization::HeaderSerializer headerSerializer;

    common::itf::Header header("GetHistory", nodeName, queName);
    common::itf::Message message(common::utils::Timestamp::get(), RequestData,
                                 clientName);

    std::string data = headerSerializer.serialize(header); + "@" +
    messageSerializer.serialize(message) + "\n";
    return this->WebSocketConnection(host, port, data);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

int Client::WebSocketConnection(std::string host, char *port,
                                std::string message) {
  try {
    auto const results = resolver.resolve(host, port);
    auto ep = net::connect(webSocketStream.next_layer(), results);
    host += ':' + std::to_string(ep.port());

    webSocketStream.set_option(
        websocket::stream_base::decorator([](websocket::request_type &req) {
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
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}