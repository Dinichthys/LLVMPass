#pragma once

#include "graph/graph.hpp"

namespace ag {

enum AnalyzerError {
    kDone            = 0,
    kInvalidArgument = 1
};

AnalyzerError AnalyzeGraph(graph::Cluster *root);

};
