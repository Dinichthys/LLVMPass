#include "graph/graph.hpp"

#include "graph/read/read_static_info.hpp"
#include "graph/read/read_dynamic_info.hpp"

#include "graph/analyze/analyze.hpp"

#include "graph/write/write_graph.hpp"

int main() {
    graph::Cluster root;

    rsi::ReadStaticInfo(&root);
    rdi::ReadDynamicInfo(&root);

    ag::AnalyzeGraph(&root);

    wg::WriteGraph(&root);
}
