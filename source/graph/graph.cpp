#include <iostream>
#include <stdlib.h>

#include "graph/graph.hpp"

#include "graph/read/read_static_info.hpp"
#include "graph/read/read_dynamic_info.hpp"

#include "graph/analyze/analyze.hpp"

#include "graph/write/write_graph.hpp"

int main() {
    graph::Cluster root;

    if (rsi::ReadStaticInfo(&root) != rsi::StaticInfoReadError::kDone) {
        std::cerr << "Error in static info\n";
        return EXIT_FAILURE;
    }

    if (rdi::ReadDynamicInfo(&root) != rdi::DynamicInfoReadError::kDone) {
        std::cerr << "Error in dynamic info\n";
        return EXIT_FAILURE;
    }

    if (ag::AnalyzeGraph(&root) != ag::AnalyzerError::kDone) {
        std::cerr << "Error in analyzing\n";
        return EXIT_FAILURE;
    }

    if (wg::WriteGraph(&root) != wg::WriteGraphError::kDone) {
        std::cerr << "Error in writing graph\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
