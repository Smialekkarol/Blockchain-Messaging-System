#pragma once
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

    void addChannel(std::string channelName)
    {
        channels.emplace_back("default", channelName);
    }

    std::vector<Channel>& getChannels()
    {
        return channels;
    }


private:
    std::vector<Channel> channels{};
};

}