#pragma once

namespace common
{
struct Channel
{
    virtual ~Channel() = default;
    std::string type;
    std::string name;
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
