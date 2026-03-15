#pragma once

#include "graph/graph.hpp"

namespace rsi {

enum StaticInfoReadError {
    kDone           = 0,
    kCantOpenFile   = 1,
    kInvalidKeyWord,
    kInvalidArgument,
    kBadAlloc
};

StaticInfoReadError ReadStaticInfo(graph::Cluster *root);

}; // namespace rsi
