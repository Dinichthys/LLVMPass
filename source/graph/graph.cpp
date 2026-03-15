#include "graph/graph.hpp"

#include "graph/read/read_static_info.hpp"
#include "graph/read/read_dynamic_info.hpp"

#include "graph/write/write_graph.hpp"

int main() {
    graph::Cluster root;
    rsi::ReadStaticInfo(&root);
    rdi::ReadDynamicInfo(&root);
    wg::WriteGraph(&root);
}
