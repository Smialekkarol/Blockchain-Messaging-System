#include <sw/redis++/redis++.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

const uint64_t getTimeStamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

struct Message
{
    Message() = default;
    Message(const uint64_t timestamp_, const std::string &data_) : timestamp{timestamp_}, data{data_} {}
    Message(const Message &other) = default;
    Message(Message &&other) = default;
    Message &operator=(const Message &other) = default;
    Message &operator=(Message &&other) = default;
    ~Message() = default;

    uint64_t timestamp{0};
    std::string data{};

    void describe() const
    {
        std::cout << "timestamp: " << timestamp << " data: " << data << std::endl;
    }
};

namespace boost::serialization
{
    template <class Archive>
    void serialize(Archive &archive, Message &message, const unsigned int version)
    {
        archive &message.timestamp;
        archive &message.data;
    }
}

struct Block
{
    Block() = default;
    Block(const std::vector<Message> &data) : timestamp{getTimeStamp()}, data{data} {}
    Block(const uint64_t timestamp, const std::vector<Message> &data) : timestamp{timestamp}, data{data} {}
    Block(const Block &other) = default;
    Block(Block &&other) = default;
    Block &operator=(const Block &other) = default;
    Block &operator=(Block &&other) = default;
    ~Block() = default;

    uint64_t timestamp{0};
    std::vector<Message> data{};

    void describe() const
    {
        std::cout << "timestamp: " << timestamp << " data: " << std::endl;
        for (int i = 0; i < data.size(); i++)
        {
            std::cout << "data[i]: ";
            data[i].describe();
        }
    }
};

namespace boost::serialization
{
    template <class Archive>
    void serialize(Archive &archive, Block &block, const unsigned int version)
    {
        archive &block.timestamp;
        archive &block.data;
    }
}

int main(int, char **)
{
    auto redis = sw::redis::Redis("tcp://127.0.0.1:6379");

    auto start = std::chrono::high_resolution_clock::now();
    std::stringstream ss{};
    Message message1{getTimeStamp(), "Lorem ipsum dolor sit amet."};
    Message message2{getTimeStamp(), "Lorem ipsum dolor sit amet."};
    Message message3{getTimeStamp(), "Lorem ipsum dolor sit amet."};
    std::vector<Message> messages = {message1, message2, message3};

    Block block{messages};
    std::cout << "Before serialization: \n";
    block.describe();

    boost::archive::text_oarchive textOutputArchive(ss);
    textOutputArchive << block;
    std::cout << "Serialized data: \n"
              << ss.str() << std::endl;

    redis.set("block1", ss.str());
    ss.str("");
    auto val1 = redis.get("block1");
    Block deserializedBlock{};
    if (val1)
    {
        ss.str(*val1);
        boost::archive::text_iarchive inputTextArchive(ss);
        inputTextArchive >> deserializedBlock;
        std::cout << "After serialization: \n";
        deserializedBlock.describe();
    }
    redis.save();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "operation duration: " << duration.count() << std::endl;
    std::cout << "total size: " << (((sizeof(std::string) + 675) * deserializedBlock.data.size()) + (sizeof(uint64_t) * deserializedBlock.data.size())) << std::endl;
}
