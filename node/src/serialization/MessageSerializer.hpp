#pragma once

#include "/root/node/src/itf/Message.hpp"

#include "Serializer.hpp"

namespace serialization {
using MessageSerializer = Serializer<::itf::Message>;
} // namespace serialization
