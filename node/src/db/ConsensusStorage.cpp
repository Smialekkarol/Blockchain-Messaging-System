#include "ConsensusStorage.hpp"
#include <algorithm>

namespace db {

std::optional<bool> ConsensusStorage::isContributing(const std::string &address,
                                                     const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  return contexts[address][slot].isContributing;
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

void ConsensusStorage::clearSlot(const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  for (auto &[_, slotContexts] : contexts) {
    slotContexts.erase(slot);
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

void ConsensusStorage::markValidator(const std::string &address,
                                     const std::uint64_t slot) {
  std::scoped_lock lock(mutex);
  contexts[address][slot].isValidator = true;
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
