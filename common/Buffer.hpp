#include <iostream>
#include <vector>

namespace common
{

template <typename T>
class Buffer
{
public:
    Buffer() = default;

    inline void addMessage(T message)
    {
        std::scoped_lock lock(mutex);

        bufferedData.push_back(message);
    }

    inline bool isEmpty()
    {
        std::scoped_lock lock(mutex);
        return bufferedData.empty();
    }

    inline int getSize()
    {
        std::scoped_lock lock(mutex);
        return bufferedData.size();
    }

    inline void clearBuffer()
    {
        std::scoped_lock lock(mutex);
        bufferedData.clear();
    }

    inline std::vector<T> getBufferedData()
    {
        std::scoped_lock lock(mutex);
        return bufferedData;
    }

    void print()
    {
        for (auto msg : bufferedData)
        {
            std::cout << "author " << msg.author << ": " << msg.data << std::endl;
        }
    }

private:
    std::vector<T> bufferedData{};
    std::mutex mutex{};
};
}