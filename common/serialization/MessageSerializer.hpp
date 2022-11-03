#pragma once

#include "common/itf/Message.hpp"

#include "Serializer.hpp"

namespace serialization {
using MessageSerializer = Serializer<::common::itf::Message>;
} // namespace serialization
