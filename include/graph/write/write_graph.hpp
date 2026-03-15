#pragma once

#include "graph/graph.hpp"

namespace wg {

enum WriteGraphError {
    kDone           = 0,
    kCantOpenFile   = 1,
    kInvalidArgument
};

static const std::string kDotFileName = "./log/tmp.dot";

WriteGraphError WriteGraph(const graph::Cluster *module);

}; // namespace wg
