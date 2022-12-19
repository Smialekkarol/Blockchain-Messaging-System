#pragma once

#include "node/src/io/ChannelStore.hpp"
#include "node/src/io/ContributionWrapperSerializer.hpp"

#include "SlotContext.hpp"

namespace slot {
class ContributionNotifier {
public:
  ContributionNotifier(SlotContext &context, ::io::ChannelStore &channelStore);
  void notify();

private:
  std::string createMessage();

  SlotContext &context;
  ::io::ChannelStore &channelStore;
  ::io::ContributionWrapperSerializer contributionWrapperSerializer{};
};

} // namespace slot