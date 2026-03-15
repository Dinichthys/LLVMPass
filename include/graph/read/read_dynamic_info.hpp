#pragma once

#include "graph/graph.hpp"

namespace rdi {

enum DynamicInfoReadError {
    kDone           = 0,
    kCantOpenFile   = 1,
    kInvalidKeyWord,
    kInvalidArgument,
    InvalidDynamicInfo
};

DynamicInfoReadError ReadDynamicInfo(graph::Cluster *module);

}; // namespace rsi
