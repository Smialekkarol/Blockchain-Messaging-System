#include <iostream>
#include <sw/redis++/redis++.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

struct Block
{
    Block() = default;
    Block(int id, std::string data) : id{id}, data{data} {}
    Block(const Block &other) : id{other.id}, data{other.data} {}
    Block(Block &&other) : id{other.id}, data{other.data} {}
    ~Block() = default;

    int id{0};
    std::string data{};

    void describe() const
    {
        std::cout << "id: " << id << " data: " << data << std::endl;
    }
};

namespace boost::serialization
{
    template <class Archive>
    void serialize(Archive &archive, Block &block, const unsigned int version)
    {
        archive &block.id;
        archive &block.data;
    }
}

int main(int, char **)
{
    using namespace sw::redis;

    try
    {
        // Create an Redis object, which is movable but NOT copyable.
        auto redis = Redis("tcp://127.0.0.1:6379");

        // ***** LIST commands *****

        std::stringstream ss{};

        const Block block(1, "Message origin");
        std::cout << "Before serialization: \n";
        block.describe();

        boost::archive::text_oarchive textOutputArchive(ss);
        textOutputArchive << block;
        std::cout << "Serialized data: " << ss.str() << std::endl;

        // std::vector<std::string> to Redis LIST.
        std::vector<std::string> vec = {ss.str()};
        std::cout << "before serialization vec size: " << vec.size() << std::endl;
        redis.rpush("list", vec.begin(), vec.end());

        // Redis LIST to std::vector<std::string>.
        vec.clear();
        redis.lrange("list", 0, -1, std::back_inserter(vec));
        std::cout << "After serialization vec size: " << vec.size() << std::endl
                  << std::endl;

        std::for_each(vec.begin(), vec.end(),
                      [&](auto data)
                      {
                          std::cout << "\n========================================\n"
                                    << data << std::endl;
                          ss.str("");
                          Block deserializedBlock{};
                          ss.str(data);
                          std::cout << "data from redis: " << data << std::endl;
                          std::cout << "data from stringstream: " << ss.str() << std::endl;
                          boost::archive::text_iarchive inputTextArchive(ss);
                          inputTextArchive >> deserializedBlock;
                          std::cout << "After serialization: \n";
                          deserializedBlock.describe();
                      });
        redis.save();
    }
    catch (const Error &e)
    {
        std::cout << "Error happend, reason: " << e.what() << std::endl;
    }
}
