#pragma once

#include <mutex>
#include <unordered_map>

#include "common/itf/Block.hpp"

#include "ConsensusContext.hpp"

namespace db {
class ConsensusStorage {
public:
  ConsensusStorage() = default;
  ConsensusStorage(const ConsensusStorage &other) = delete;
  ConsensusStorage(ConsensusStorage &&other) = delete;
  ConsensusStorage &operator=(const ConsensusStorage &other) = delete;
  ConsensusStorage &operator=(ConsensusStorage &&other) = delete;
  ~ConsensusStorage() = default;

  std::optional<bool> isContributing(const std::string &address,
                                     const std::uint64_t slot);
  std::optional<ConsensusContext> findValidator(const std::uint64_t slot);
  void addContext(const std::string &address, const std::uint64_t slot,
                  const std::string &node);
  void setContribution(const std::string &address, const std::uint64_t slot,
                       bool isContributing);
  void markValidator(const std::string &address, const std::uint64_t slot);
  void fillElectionValue(const std::string &address, const std::uint64_t slot,
                         const std::uint64_t electionValue);
  void addBlock(const std::string &address, const std::uint64_t slot,
                const ::common::itf::Block &block);
  void marAsResolved(const std::string &address, const std::uint64_t slot);

private:
  std::unordered_map<std::string,
                     std::unordered_map<std::uint64_t, ConsensusContext>>
      contexts{};
  std::mutex mutex{};
};
} // namespace db
