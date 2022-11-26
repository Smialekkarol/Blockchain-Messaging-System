#include <iostream>
#include <vector>

namespace common {

template <typename T> class Buffer {
public:
  Buffer() = default;

  inline void addMessage(T message) { bufferedData.push_back(message); }
  inline bool isEmpty() const { return bufferedData.empty(); }
  inline int getSize() const { return bufferedData.size(); }
  inline void clearBuffer() { bufferedData.clear(); }
  inline std::vector<T> getBufferedData() { return bufferedData; }

  void print() {
    for (auto msg : bufferedData) {
      std::cout << "author " << msg.author << ": " << msg.data << std::endl;
    }
  }

private:
  std::vector<T> bufferedData{};
};
}