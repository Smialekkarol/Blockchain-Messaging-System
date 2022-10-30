#pragma once

#include "itf/Message.hpp"

#include "Serializer.hpp"

namespace serialization {
using MessageSerializer = Serializer<::itf::Message>;
} // namespace serialization
