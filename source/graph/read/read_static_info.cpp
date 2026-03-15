#include "graph/read/read_static_info.hpp"

#include <fstream>

#include "graph/graph.hpp"
#include "pass/static_info_constants.hpp"

namespace rsi {

enum KeyWord {
    kInvalid = -1,
    kCluster = 0,
    kNode,
    kEdge,
    kEndBlock
};

static StaticInfoReadError ReadStaticInfoFromFile(graph::Cluster *root, std::ifstream &in);

static StaticInfoReadError ReadCluster(graph::Cluster *root, std::ifstream &in);
static StaticInfoReadError ReadSubCluster(graph::Cluster *root, std::ifstream &in);
static StaticInfoReadError ReadNode(graph::Cluster *root, std::ifstream &in);
static StaticInfoReadError ReadEdge(graph::Cluster *root, std::ifstream &in);

static KeyWord GetKeyWordEnum(const std::string &key_word);

StaticInfoReadError ReadStaticInfo(graph::Cluster *root) {
    if (root == NULL) {
        return StaticInfoReadError::kInvalidArgument;
    }

    std::ifstream in(sinfo::kStaticInfoFileName);
    if (!in.is_open()) {
        return StaticInfoReadError::kCantOpenFile;
    }

    std::string key_word_str;
    std::getline(in, key_word_str);
    if (key_word_str != sinfo::kKeyWordStartClusterBlock) {
        return StaticInfoReadError::kInvalidKeyWord;
    }

    return ReadCluster(root, in);
}

static StaticInfoReadError ReadStaticInfoFromFile(graph::Cluster *root, std::ifstream &in) {
    StaticInfoReadError error = StaticInfoReadError::kDone;
    while (!in.eof()) {
        std::string key_word_str;
        std::getline(in, key_word_str);
        KeyWord key_word = GetKeyWordEnum(key_word_str);

        switch (key_word) {
            case KeyWord::kCluster : {
                error = ReadSubCluster(root, in);
                if (error != StaticInfoReadError::kDone) {
                    return error;
                }
                break;
            };

            case KeyWord::kNode : {
                error = ReadNode(root, in);
                if (error != StaticInfoReadError::kDone) {
                    return error;
                }
                break;
            };

            case KeyWord::kEdge : {
                error = ReadEdge(root, in);
                if (error != StaticInfoReadError::kDone) {
                    return error;
                }
                break;
            };

            case KeyWord::kEndBlock : {
                return error;
            };

            case KeyWord::kInvalid : {
                return StaticInfoReadError::kInvalidKeyWord;
            };
        };
    }

    return error;
}

static KeyWord GetKeyWordEnum(const std::string &key_word) {
    if (key_word == sinfo::kKeyWordStartClusterBlock) {
        return KeyWord::kCluster;
    }
    if (key_word == sinfo::kKeyWordStartNodeBlock) {
        return KeyWord::kNode;
    }
    if (key_word == sinfo::kKeyWordStartEdgeBlock) {
        return KeyWord::kEdge;
    }
    if (key_word == sinfo::kKeyWordEndBlock) {
        return KeyWord::kEndBlock;
    }
    return KeyWord::kInvalid;
}

static StaticInfoReadError ReadCluster(graph::Cluster *root, std::ifstream &in) {
    std::string cluster_name, cluster_color, cluster_label, cluster_border_color;
    std::getline(in, cluster_name);
    std::getline(in, cluster_color);
    std::getline(in, cluster_label);
    std::getline(in, cluster_border_color);

    root->SetName(cluster_name);
    root->SetColor(cluster_color);
    root->SetLabel(cluster_label);
    root->SetBorderColor(cluster_border_color);

    return ReadStaticInfoFromFile(root, in);
}

static StaticInfoReadError ReadSubCluster(graph::Cluster *root, std::ifstream &in) {
    graph::Cluster *subcluster = new(std::nothrow) graph::Cluster;
    if (subcluster == nullptr) {
        return StaticInfoReadError::kBadAlloc;
    }

    root->AddChild(subcluster);

    return ReadCluster(subcluster, in);
}

static StaticInfoReadError ReadNode(graph::Cluster *root, std::ifstream &in) {
    std::string node_name, node_label, node_color, node_border_color;
    std::getline(in, node_name);
    std::getline(in, node_label);
    std::getline(in, node_color);
    std::getline(in, node_border_color);

    graph::Node *node = new(std::nothrow) graph::Node;
    if (node == nullptr) {
        return StaticInfoReadError::kBadAlloc;
    }

    node->SetName(node_name);
    node->SetColor(node_color);
    node->SetLabel(node_label);
    node->SetBorderColor(node_border_color);

    root->AddChild(node);

    return StaticInfoReadError::kDone;
}

static StaticInfoReadError ReadEdge(graph::Cluster *root, std::ifstream &in) {
    std::string start, end, color;
    std::getline(in, start);
    std::getline(in, end);
    std::getline(in, color);

    graph::Edge edge{};
    edge.SetStart(start);
    edge.SetColor(color);
    edge.SetEnd(end);

    root->AddEdge(std::move(edge));

    return StaticInfoReadError::kDone;
}

}; // namespace rsi
