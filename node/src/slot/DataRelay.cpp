#include <spdlog/spdlog.h>

#include "node/src/io/Utils.hpp"

#include "DataRelay.hpp"

namespace slot {
DataRelay::DataRelay(SlotContext &context, ::io::ChannelStore &channelStore,
                     ::db::ConsensusStorage &consensusStorage)
    : context{context}, channelStore{channelStore}, consensusStorage{
                                                        consensusStorage} {}

void DataRelay::transfer() {
  context.header.operation = ::io::ConsensusOperation::UPLOADING;
  context.serializedHeader = headerSerializer.serialize(context.header);

  context.validator = consensusStorage.getValidator(context.header.slot);
  context.isValidator =
      context.validator == context.nodeConfiguration.self.address;

  if (context.isValidator || !context.contributionWrapper.isContributing) {
    spdlog::debug("[{}] DataRelay::transfer i am not sending data",
                 context.header.slot);
    return;
  }

  consensusStorage.addBlock(context.nodeConfiguration.self.address,
                            context.header.slot, context.block);
  const auto &message = createMessage();
  auto &validatorChannel = channelStore.getRemote(context.validator);
  spdlog::debug("[{}] DataRelay::transfer sending data: {}", context.header.slot,
               message);
  validatorChannel.publish(message);
  consensusStorage.marAsResolved(context.nodeConfiguration.self.address,
                                 context.header.slot);
}

std::string DataRelay::createMessage() {
  return ::io::merge(context.serializedHeader,
                     blockSerializer.serialize(context.block));
}
} // namespace slot