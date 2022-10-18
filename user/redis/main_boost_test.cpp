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

int main()
{
    std::stringstream ss{};

    const Block block(1, "Message origin");
    std::cout << "Before serialization: \n";
    block.describe();

    boost::archive::text_oarchive textOutputArchive(ss);
    textOutputArchive << block;
    std::cout << "Serialized data: " << ss.str() << std::endl;

    Block deserializedBlock{};
    boost::archive::text_iarchive inputTextArchive(ss);
    inputTextArchive >> deserializedBlock;
    std::cout << "After serialization: \n";
    deserializedBlock.describe();

    return 0;
}