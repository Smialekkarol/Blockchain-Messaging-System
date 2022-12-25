#include <algorithm>
#include <numeric>
#include <set>

#include <spdlog/spdlog.h>

#include "ConsensusStorage.hpp"

namespace db {
ConsensusStorage::ConsensusStorage(const ::common::NodeConfiguration &config)
    : config{config} {}

bool ConsensusStorage::areAllContributorsConfirmed(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  return std::all_of(contexts.begin(), contexts.end(),
                     [this, slot](auto &pair) {
                       return pair.second[slot].isContributing.has_value();
                     });
}

bool ConsensusStorage::isAnyNodeContributing(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  return std::any_of(contexts.begin(), contexts.end(),
                     [this, slot](auto &pair) {
                       return pair.second[slot].isContributing.value();
                     });
}

bool ConsensusStorage::areAllElectionValuesPresent(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  return std::all_of(contexts.begin(), contexts.end(), [slot](auto &pair) {
    return pair.second[slot].electionValue > 0;
  });
}

bool ConsensusStorage::isElectionValueUnique(const std::string &address,
                                             const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  const auto electionValue = contexts[address][slot].electionValue;
  bool isElectionValueUnique{true};
  for (auto &[contextAddress, consensusContexts] : contexts) {
    if (contextAddress == address) {
      continue;
    }
    const auto value = consensusContexts[slot].electionValue;
    if (value == electionValue) {
      spdlog::error("Duplicated elevtion value [{}] for slot [{}]",
                    electionValue, slot);
      isElectionValueUnique = false;
      break;
    }
  }
  return isElectionValueUnique;
}

void ConsensusStorage::removeElectionValueDuplicates(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  std::set<std::string> duplicatedAddresses{};
  for (auto &[address, consensusContexts] : contexts) {
    bool isAnyDuplicate{false};
    const auto value = consensusContexts[slot].electionValue;
    for (auto &[addressCompared, consensusContextsCompared] : contexts) {
      const auto compared = consensusContextsCompared[slot].electionValue;
      if (value == compared) {
        isAnyDuplicate = true;
        duplicatedAddresses.emplace(address);
      }
    }
    if (isAnyDuplicate) {
      duplicatedAddresses.emplace(address);
    }
  }

  for (auto &address : duplicatedAddresses) {
    contexts[address][slot].electionValue = 0;
  }
}

std::optional<ConsensusContext>
ConsensusStorage::findValidator(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  const auto &it =
      std::find_if(contexts.begin(), contexts.end(), [slot](auto slotContexts) {
        return slotContexts.second[slot].isValidator;
      });
  if (it != contexts.end()) {
    return it->second[slot];
  }
  return {};
}

void ConsensusStorage::initContexts(const std::uint64_t slot) {
  contexts[config.self.address].try_emplace(slot, ConsensusContext{});
  for (const auto &node : config.nodes) {
    contexts[node.address].try_emplace(slot, ConsensusContext{});
  }
}

void ConsensusStorage::initConditions(const std::uint64_t slot) {
  const auto &isContextFound =
      std::find_if(conditions.begin(), conditions.end(),
                   [slot](const auto &pair) { return pair.first == slot; });
  if (isContextFound != conditions.end()) {
    return;
  }
  conditions.emplace(slot, std::make_shared<SlotSychronizationContext>());
}

std::shared_ptr<SlotSychronizationContext>
ConsensusStorage::getSlotSynchronizationContext(const std::uint64_t slot) {
  return conditions[slot];
}

void ConsensusStorage::clearSlot(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  for (auto &[_, slotContexts] : contexts) {
    slotContexts.erase(slot);
    conditions.erase(slot);
  }
}

void ConsensusStorage::addContext(const std::string &address,
                                  const std::string &node,
                                  const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  contexts[address][slot].node = node;
}

void ConsensusStorage::setContribution(const std::string &address,
                                       const std::uint64_t slot,
                                       bool isContributing) {
  std::scoped_lock lock(mutex);
  contexts[address][slot].isContributing = isContributing;
}

void ConsensusStorage::markValidator(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  std::for_each(contexts.begin(), contexts.end(), [slot](auto &pair) {
    spdlog::info("Election Value: {}", pair.second[slot].electionValue);
  });
  const auto sum = std::accumulate(
      contexts.begin(), contexts.end(), 0, [slot](auto prev, auto &pair) {
        return prev + pair.second[slot].electionValue;
      });
  spdlog::info("Election Values sum: {}", sum);
  std::string_view validator{contexts.begin()->first};
  std::uint64_t value{contexts.begin()->second[slot].electionValue};
  if (sum % 2) {
    spdlog::info("Election Values are even, choosing the biggest value");
    std::for_each(contexts.begin(), contexts.end(),
                  [&validator, &value, slot](auto &pair) {
                    if (const auto v = pair.second[slot].electionValue;
                        v > value) {
                      validator = pair.first;
                      value = v;
                    }
                  });
  } else {
    spdlog::info("Election Values are odd, choosing the smallest value");
    std::for_each(contexts.begin(), contexts.end(),
                  [&validator, &value, slot](auto &pair) {
                    if (const auto v = pair.second[slot].electionValue;
                        v < value) {
                      validator = pair.first;
                      value = v;
                    }
                  });
  }
  spdlog::info("Validator is: {} with value: {}", validator.data(),
               contexts[validator.data()][slot].electionValue);
  contexts[validator.data()][slot].isValidator = true;
}

void ConsensusStorage::fillElectionValue(const std::string &address,
                                         const std::uint64_t slot,
                                         const std::uint64_t electionValue) {
  std::scoped_lock lock(mutex);
  contexts[address][slot].electionValue = electionValue;
}

void ConsensusStorage::addBlock(const std::string &address,
                                const std::uint64_t slot,
                                const ::common::itf::Block &block) {
  std::scoped_lock lock(mutex);
  contexts[address][slot].block = block;
}

void ConsensusStorage::marAsResolved(const std::string &address,
                                     const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  contexts[address][slot].isResolved = true;
}

} // namespace db
