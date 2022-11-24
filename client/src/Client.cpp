#include "Client.hpp"

namespace client {

Client::Client(net::io_context &ioc, ClientInfo &clientInfo)
    : clientInfo(clientInfo), websocketHandler((ioc)) {
  websocketHandler.EstablishConnection(clientInfo);

  auto callable = [ this]() {
    Listen();
    websocketHandler.clearBuffer();
  };
  Timer<std::function<void()>> timer;
timer.setInterval(callable, 200);
}

Client::~Client() { std::cout << "~Client()\n"; }

int Client::MakeInitialConnection(std::string queToBeCreated) {
  try {
    std::string queName = clientInfo.serverName + "_ControlChannel";

    common::itf::Header header("CreateChannel", clientInfo.serverName, queName,
                               clientInfo.clientName);
    common::itf::Message message(common::utils::Timestamp::get(),
                                 queToBeCreated, clientInfo.clientName);

    serialization::MessageSerializer messageSerializer;
    serialization::HeaderSerializer headerSerializer;

    auto data = headerSerializer.serialize(header) + "@" +
                messageSerializer.serialize(message) + "\n";

    websocketHandler.writeDataToWebsocket(data);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

int Client::SendMessage(std::string queName, std::string dataToSend) {
  try {
    serialization::MessageSerializer messageSerializer;
    serialization::HeaderSerializer headerSerializer;

    common::itf::Header header("Send", clientInfo.serverName, queName,
                               clientInfo.clientName);
    common::itf::Message message(common::utils::Timestamp::get(), dataToSend,
                                 clientInfo.clientName);

    std::string data = headerSerializer.serialize(header) + "@" +
                       messageSerializer.serialize(message) + "\n";

    websocketHandler.writeDataToWebsocket(data);

    return EXIT_SUCCESS;
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

int Client::getData(std::string queName, std::string RequestData) {
  try {
    serialization::MessageSerializer messageSerializer;
    serialization::HeaderSerializer headerSerializer;

    common::itf::Header header("GetHistory", clientInfo.serverName, queName,
                               clientInfo.clientName);
    common::itf::Message message(common::utils::Timestamp::get(), RequestData,
                                 clientInfo.clientName);

    std::string data = headerSerializer.serialize(header);
    +"@" + messageSerializer.serialize(message) + "\n";
    websocketHandler.writeDataToWebsocket(data);

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}

void Client::Listen() {
  websocketHandler.readDataFromWebsocket();
  std::vector<std::string> bufferedData = common::utils::Text::splitBySeparator(
      websocketHandler.getBufferedData(), "@");
      websocketHandler.clearBuffer();
  serialization::MessageSerializer messageSerializer;
  std::string serializedData{};
  for (auto msg : bufferedData) {
    auto messageObject = messageSerializer.deserialize(msg);
    std::cout << messageObject.author + ": " + messageObject.data + "\n"
              << std::endl;
  }

}
}