#pragma once
#include <vector>

namespace common
{

template <typename dataType>
class Buffer
{
public:

    void pushBack(dataType message)
    {
        std::scoped_lock lock(mutex);
        data.push_back(message);
    }

    bool isEmpty()
    {
        std::scoped_lock lock(mutex);
        return data.empty();
    }

    inline int getSize()
    {
        std::scoped_lock lock(mutex);
        return data.size();
    }

    inline void clearBuffer()
    {
        std::scoped_lock lock(mutex);
        data.clear();
    }

    inline std::vector<dataType> getData()
    {
        std::scoped_lock lock(mutex);
        return data;
    }

private:
    std::vector<dataType> data{};
    std::mutex mutex{};
};
}