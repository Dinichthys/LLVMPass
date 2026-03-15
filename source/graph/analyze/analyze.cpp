#include "graph/analyze/analyze.hpp"

#include <assert.h>
#include <format>

#include "graph/graph.hpp"

namespace ag {

static const size_t kByteSize = 8;

static const size_t kMaxColor = 0xff;

static const size_t kRedColor   = 0xff0000;
static const size_t kGreenColor = 0x00ff00;
static const size_t kBlueColor  = 0x0000ff;

static const char kHexColorSym = '#';

static AnalyzerError AnalyzeCluster(graph::Cluster *root);

static void ChangeClusterColor(graph::Cluster *root, size_t max_entrance);

AnalyzerError AnalyzeGraph(graph::Cluster *root) {
    if (root == NULL) {
        return AnalyzerError::kInvalidArgument;
    }

    return AnalyzeCluster(root);
}

static AnalyzerError AnalyzeCluster(graph::Cluster *root) {
    assert(root);

    size_t max_entrance = 0;

    std::vector<graph::Cluster*> subclusters = {};
    for (auto *node : root->GetChildren()) {
        graph::Cluster *subcluster = dynamic_cast<graph::Cluster*>(node);
        if (subcluster == nullptr) {
            continue;
        }

        subclusters.push_back(subcluster);

        max_entrance = std::max(max_entrance, subcluster->GetEntrance());
    }

    for (auto *subcluster : subclusters) {
        AnalyzeCluster(subcluster);

        ChangeClusterColor(subcluster, max_entrance);
    }

    return AnalyzerError::kDone;
}

static void ChangeClusterColor(graph::Cluster *root, size_t max_entrance) {
    assert(root);

    size_t entrance_num = root->GetEntrance();

    const std::string &color = root->GetColor();
    if (color[0] != kHexColorSym) {
        return;
    }

    size_t color_num = std::stoi(color.substr(1, color.length() - 1).c_str(), nullptr, 16);

    size_t new_color_num = color_num
                            + (((kMaxColor - ((color_num & kRedColor) >> (kByteSize * 2)))
                                * entrance_num / max_entrance) << (kByteSize * 2))
                            + ((kMaxColor - (color_num & kBlueColor))
                                * (max_entrance - entrance_num) / max_entrance);

    root->SetColor(kHexColorSym + std::format("{:06X}", new_color_num));
}



}; // namespace ag
