#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <ev.h>

class ConnectionHandler : public AMQP::LibEvHandler {
private:
  virtual void onError(AMQP::TcpConnection *connection,
                       const char *message) override {
    std::cout << "error: " << message << std::endl;
  }

  virtual void onConnected(AMQP::TcpConnection *connection) override {}

  virtual void onClosed(AMQP::TcpConnection *connection) override {}

public:
  ConnectionHandler(struct ev_loop *loop) : AMQP::LibEvHandler(loop) {}
  virtual ~ConnectionHandler() = default;
};