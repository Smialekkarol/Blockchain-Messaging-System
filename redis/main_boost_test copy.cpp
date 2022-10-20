#include <fstream>
#include <sstream>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

struct Block
{
    int id{0};
    std::string data{};
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

/////////////////////////////////////////////////////////////
// gps coordinate
//
// illustrates serialization for a simple type
//
class gps_position
{
private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar &degrees;
        ar &minutes;
        ar &seconds;
    }
    int degrees;
    int minutes;
    float seconds;

public:
    gps_position(){};
    gps_position(int d, int m, float s) : degrees(d), minutes(m), seconds(s)
    {
    }

    void describe() const
    {
        std::cout << "degrees: " << degrees << " minutes: " << minutes << " seconds: " << seconds << std::endl;
    }
};

int main()
{
    // create and open a character archive for output
    std::stringstream ss{};

    // create class instance
    const gps_position g(35, 59, 24.567f);
    std::cout << "Before serialization: \n";
    g.describe();

    // save data to archive
    {
        boost::archive::text_oarchive oa(ss);
        // write class instance to archive
        oa << g;
        // archive and stream closed when destructors are called
    }

    // ... some time later restore the class instance to its orginal state
    gps_position newg;
    {
        // create and open an archive for input
        boost::archive::text_iarchive ia(ss);
        // read class state from archive
        ia >> newg;
        // archive and stream closed when destructors are called
        std::cout << "After serialization: \n";
        newg.describe();
    }
    return 0;
}