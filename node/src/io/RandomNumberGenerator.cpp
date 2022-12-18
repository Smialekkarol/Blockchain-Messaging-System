#include "RandomNumberGenerator.hpp"

namespace io {
std::uint64_t RandomNumberGenerator::generate() { return generator(engine); }
} // namespace io