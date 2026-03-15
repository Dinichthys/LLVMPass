#include "graph/write/write_graph.hpp"

#include <fstream>
#include <assert.h>

namespace wg {

static const std::string kEdgeWeight = std::to_string(10);

static void WriteGraphInfo(const graph::Cluster *module, std::ofstream &out, bool show_entrance);

static void WriteNodes(const graph::Cluster *cluster, std::ofstream &out);
static void WriteEdges(const graph::Cluster *cluster, std::ofstream &out);

static void WriteStartGraph(std::ofstream &out);
static void WriteEndGraph(std::ofstream &out);

static void WriteClusterStart(const graph::Cluster *cluster, std::ofstream &out, bool show_entrance);
static void WriteClusterEnd(std::ofstream &out);

static void WriteNodeInfo(const graph::Node *node, std::ofstream &out);
static void WriteEdgeInfo(const graph::Edge &edge, std::ofstream &out);

WriteGraphError WriteGraph(const graph::Cluster *module) {
    if (module == NULL) {
        return WriteGraphError::kInvalidArgument;
    }

    std::ofstream out(kDotFileName);
    if (!out.is_open()) {
        return WriteGraphError::kCantOpenFile;
    }

    WriteStartGraph(out);

    WriteGraphInfo(module, out, false);

    WriteEndGraph(out);

    return WriteGraphError::kDone;
}

static void WriteGraphInfo(const graph::Cluster *cluster, std::ofstream &out, bool show_entrance) {
    assert(cluster);

    WriteClusterStart(cluster, out, show_entrance);

    WriteNodes(cluster, out);

    WriteEdges(cluster, out);

    WriteClusterEnd(out);
}

static void WriteNodes(const graph::Cluster *cluster, std::ofstream &out) {
    assert(cluster);

    for (const auto *node : cluster->GetChildren()) {
        const graph::Cluster *subcluster = dynamic_cast<const graph::Cluster*>(node);
        if (subcluster) {
            WriteGraphInfo(subcluster, out, true);
        } else {
            WriteNodeInfo(node, out);
        }
    }
}

static void WriteEdges(const graph::Cluster *cluster, std::ofstream &out) {
    assert(cluster);

    for (const auto &edge : cluster->GetEdges()) {
        WriteEdgeInfo(edge, out);
    }
}

static void WriteStartGraph(std::ofstream &out) {
    out << "digraph\n{\n"
            "\tfontname = \"Helvetica,Arial,sans-serif\";\n"
            "\tnode [fontname = \"Helvetica,Arial,sans-serif\"];\n"
            "\tgraph [rankdir = \"TB\"];\n"
            "\tranksep = 1.5;\n"
            "\tsplines = ortho;\n\n";
}

static void WriteEndGraph(std::ofstream &out) {
    out << "}\n";
}

static void WriteClusterStart(const graph::Cluster *cluster, std::ofstream &out, bool show_entrance) {
    assert(cluster);

    out << "\tsubgraph " << cluster->GetName() << " {\n"
            "\t\tnode [style=filled];\n"
            "\t\tstyle=\"filled\";\n"
            "\t\tfillcolor=\"" << cluster->GetColor() << "\";\n"
            "\t\tlabel = \"" << cluster->GetLabel();
    if (show_entrance) {
        out << "\nrun: " << cluster->GetEntrance() << " times";
    }
    out << "\";\n\t\tcolor=\"" << cluster->GetBorderColor() << "\";\n";
}

static void WriteClusterEnd(std::ofstream &out) {
    out << "\t}\n";
}

static void WriteNodeInfo(const graph::Node *node, std::ofstream &out) {
    assert(node);

    out << "\t\"" << node->GetName() << "\"\n\t[\n"
            "\t\tlabel = \"" << node->GetLabel() << "\"\n"
            "\t\tshape = \"record\"\n"
            "\t\tstyle = \"filled\"\n"
            "\t\tfillcolor = \"" << node->GetColor() << "\"\n"
            "\t\tcolor = \"" << node->GetBorderColor() << "\"\n"
            "\t];\n\n";
}

static void WriteEdgeInfo(const graph::Edge &edge, std::ofstream &out) {
    out << "\t\"" << edge.GetStart() << "\" -> \""  << edge.GetEnd() << "\""
            "[color = \"" << edge.GetColor() << "\", weight = " << kEdgeWeight << "]\n";
}

}; // namespace wg
