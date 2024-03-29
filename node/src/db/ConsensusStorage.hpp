#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include "common/NodeConfiguration.hpp"
#include "common/itf/Block.hpp"

#include "ConsensusContext.hpp"

namespace db {

struct SlotSychronizationContext {
  SlotSychronizationContext() = default;
  SlotSychronizationContext(const SlotSychronizationContext &other) = delete;
  SlotSychronizationContext(SlotSychronizationContext &&other) = delete;
  SlotSychronizationContext &
  operator=(const SlotSychronizationContext &other) = delete;
  SlotSychronizationContext &
  operator=(SlotSychronizationContext &&other) = delete;
  ~SlotSychronizationContext() = default;

  std::condition_variable condition{};
  std::mutex mutex{};
  std::atomic<bool> isSynchronized{false};
};

class ConsensusStorage {
public:
  ConsensusStorage() = default;
  ConsensusStorage(const ::common::NodeConfiguration &nodeConfiguration);
  ConsensusStorage(const ConsensusStorage &other) = delete;
  ConsensusStorage(ConsensusStorage &&other) = delete;
  ConsensusStorage &operator=(const ConsensusStorage &other) = delete;
  ConsensusStorage &operator=(ConsensusStorage &&other) = delete;
  ~ConsensusStorage() = default;

  void initConditions(const std::uint64_t slot);
  void initContexts(const std::uint64_t slot);
  bool areAllContributorsConfirmed(const std::uint64_t slot);
  bool isAnyNodeContributing(const std::uint64_t slot);
  bool areAllElectionValuesPresent(const std::uint64_t slot);
  bool isElectionValueUnique(const std::string &address,
                             const std::uint64_t slot);
  std::string getValidator(const std::uint64_t slot);
  void removeElectionValueDuplicates(const std::uint64_t slot);
  bool isAllContributorsDataCollected(const std::uint64_t slot);
  bool isResolved(const std::string &address, const std::uint64_t slot);
  ::common::itf::Block getBlock(const std::string &address, const std::uint64_t slot);

  void clearSlot(const std::uint64_t slot);

  std::shared_ptr<SlotSychronizationContext>
  getSlotSynchronizationContext(const std::uint64_t slot);

  void addContext(const std::string &address, const std::string &node,
                  const std::uint64_t slot);
  void setContribution(const std::string &address, const std::uint64_t slot,
                       bool isContributing);
  void markValidator(const std::uint64_t slot);
  void fillElectionValue(const std::string &address, const std::uint64_t slot,
                         const std::uint64_t electionValue);
  void addBlock(const std::string &address, const std::uint64_t slot,
                const ::common::itf::Block &block);
  void marAsResolved(const std::string &address, const std::uint64_t slot);
  ::common::itf::Block mergeContributors(const std::uint64_t slot);

private:
  std::string getValidatorimpl(const std::uint64_t slot);

  ::common::NodeConfiguration nodeConfiguration;
  std::unordered_map<std::string,
                     std::unordered_map<std::uint64_t, ConsensusContext>>
      contexts{};
  std::unordered_map<std::uint64_t, std::shared_ptr<SlotSychronizationContext>>
      conditions{};
  std::mutex mutex{};
};
} // namespace db
