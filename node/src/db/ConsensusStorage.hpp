#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>

#include "common/NodeConfiguration.hpp"
#include "common/itf/Block.hpp"

#include "ConsensusContext.hpp"

// TODO: Remove usage of brackets operator in favor of .at() usage. Add fill
// method to be called at each slot beginning.
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
  ConsensusStorage(const ::common::NodeConfiguration &config);
  ConsensusStorage(const ConsensusStorage &other) = delete;
  ConsensusStorage(ConsensusStorage &&other) = delete;
  ConsensusStorage &operator=(const ConsensusStorage &other) = delete;
  ConsensusStorage &operator=(ConsensusStorage &&other) = delete;
  ~ConsensusStorage() = default;

  bool areAllContributorsConfirmed(const std::uint64_t slot);
  bool isAnyNodeContributing(const std::uint64_t slot);
  bool areAllElectionValuesPresent(const std::uint64_t slot);
  bool isElectionValueUnique(const std::string &address,
                             const std::uint64_t slot);
  void removeElectionValueDuplicates(const std::uint64_t slot);
  std::optional<ConsensusContext> findValidator(const std::uint64_t slot);
  void init(const std::uint64_t slot);
  void initContexts(const std::uint64_t slot);
  void clearSlot(const std::uint64_t slot);

  std::shared_ptr<SlotSychronizationContext>
  getSlotSynchronizationContext(const std::uint64_t slot);

  void addContext(const std::string &address, const std::string &node,
                  const std::uint64_t slot);
  void setContribution(const std::string &address, const std::uint64_t slot,
                       bool isContributing);
  void markValidator(const std::string &address, const std::uint64_t slot);
  void fillElectionValue(const std::string &address, const std::uint64_t slot,
                         const std::uint64_t electionValue);
  void addBlock(const std::string &address, const std::uint64_t slot,
                const ::common::itf::Block &block);
  void marAsResolved(const std::string &address, const std::uint64_t slot);

private:
  ::common::NodeConfiguration config;
  std::unordered_map<std::string,
                     std::unordered_map<std::uint64_t, ConsensusContext>>
      contexts{};
  std::unordered_map<std::uint64_t, std::shared_ptr<SlotSychronizationContext>>
      conditions{};
  std::mutex mutex{};
};
} // namespace db
