#pragma once

namespace common
{
struct Channel
{
    Channel(std::string type, std::string channelName)
        : type(type)
        , channelName(channelName)
    {
    }

    std::string toString()
    {
        return "channelType " + type + " channelName " + channelName + "\n";
    }

    std::string type{};
    std::string channelName{};
};

struct ClientChannel : public Channel
{
    ClientChannel(std::string channelName)
        : Channel("client", channelName + "_ClientChannel")
    {
    }
};

struct NodeControlChannel : public Channel
{
    NodeControlChannel(std::string channelName)
        : Channel("NodeControl", channelName + "_ControlChannel")
    {
    }
};
}
