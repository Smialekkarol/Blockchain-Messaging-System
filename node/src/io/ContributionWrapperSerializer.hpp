#pragma once

#include "ContributionWrapper.hpp"

#include "common/serialization/Serializer.hpp"

namespace io {
using ContributionWrapperSerializer =
    ::serialization::Serializer<ContributionWrapper>;
} // namespace io
