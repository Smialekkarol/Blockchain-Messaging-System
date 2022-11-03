#pragma once

#include "common/itf/Block.hpp"

#include "Serializer.hpp"

namespace serialization {
using BlockSerializer = Serializer<::common::itf::Block>;
} // namespace serialization
