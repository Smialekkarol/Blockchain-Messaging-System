#include <algorithm>
#include <numeric>
#include <set>

#include <spdlog/spdlog.h>

#include "ConsensusStorage.hpp"

namespace db {
ConsensusStorage::ConsensusStorage(
    const ::common::NodeConfiguration &nodeConfiguration)
    : nodeConfiguration{nodeConfiguration} {}

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

std::string ConsensusStorage::getValidator(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  return getValidatorimpl(slot);
}

std::string ConsensusStorage::getValidatorimpl(const std::uint64_t slot) {
  std::string_view validator{};
  std::for_each(contexts.begin(), contexts.end(),
                [&validator, slot](auto &pair) {
                  if (pair.second[slot].isValidator) {
                    validator = pair.first;
                  }
                });
  return validator.data();
}

bool ConsensusStorage::isAllContributorsDataCollected(
    const std::uint64_t slot) {
  return std::all_of(contexts.begin(), contexts.end(), [slot](auto &pair) {
    if (pair.second[slot].isContributing.value()) {
      return pair.second[slot].block.data.size() > 0;
    }
    return true;
  });
}

bool ConsensusStorage::isResolved(const std::string &address,
                                  const std::uint64_t slot) {
  spdlog::info("isResolved address: {}, slot: {}, isResolved: {}", address,
               slot, contexts[address][slot].isResolved);

  return contexts[address][slot].isResolved;
}

::common::itf::Block ConsensusStorage::getBlock(const std::string &address,
                                                const std::uint64_t slot) {
  return contexts[address][slot].block;
}

void ConsensusStorage::initContexts(const std::uint64_t slot) {
  contexts[nodeConfiguration.self.address].try_emplace(slot,
                                                       ConsensusContext{});
  for (const auto &node : nodeConfiguration.nodes) {
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
  const auto sum = std::accumulate(
      contexts.begin(), contexts.end(), 0, [slot](auto prev, auto &pair) {
        return prev + pair.second[slot].electionValue;
      });
  std::string_view validator{contexts.begin()->first};
  std::uint64_t value{contexts.begin()->second[slot].electionValue};
  if (sum % 2) {
    std::for_each(contexts.begin(), contexts.end(),
                  [&validator, &value, slot](auto &pair) {
                    if (const auto v = pair.second[slot].electionValue;
                        v > value) {
                      validator = pair.first;
                      value = v;
                    }
                  });
  } else {
    std::for_each(contexts.begin(), contexts.end(),
                  [&validator, &value, slot](auto &pair) {
                    if (const auto v = pair.second[slot].electionValue;
                        v < value) {
                      validator = pair.first;
                      value = v;
                    }
                  });
  }
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

::common::itf::Block
ConsensusStorage::mergeContributors(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  const auto &validator = getValidatorimpl(slot);
  auto &validatorBlock = contexts[validator][slot].block;

  std::for_each(contexts.begin(), contexts.end(),
                [&validator, &validatorBlock, slot](auto &pair) {
                  auto &context = pair.second[slot];
                  if (pair.first == validator ||
                      !context.isContributing.value()) {
                    return;
                  }
                  validatorBlock.data.insert(validatorBlock.data.end(),
                                             context.block.data.begin(),
                                             context.block.data.end());
                });

  std::sort(validatorBlock.data.begin(), validatorBlock.data.end(),
            [](const auto &message1, const auto &message2) {
              return message1.timestamp < message2.timestamp;
            });
  return validatorBlock;
}
} // namespace db
