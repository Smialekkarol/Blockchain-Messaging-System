#pragma once
#include <vector>
#include "Channel.hpp"

namespace common
{

class ChannelStore
{
public:
    ChannelStore() = default;

    void addChannel(Channel channel)
    {
        channels.push_back(channel);
    }

    std::vector<Channel>& getChannels()
    {
        return channels;
    }

private:
    std::vector<Channel> channels{};
};

}