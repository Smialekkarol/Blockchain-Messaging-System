#pragma once

#include "ElectionValueWrapper.hpp"

#include "common/serialization/Serializer.hpp"

namespace io {
using ElectionValueWrapperSerializer =
    ::serialization::Serializer<ElectionValueWrapper>;
} // namespace io
