#pragma once

#include <vector>
#include <string>

namespace graph {

class Edge {
    private:
        std::string start_;
        std::string end_;
        std::string color_;

    public:
        Edge()
            :start_(), end_(), color_() {};

        void SetStart(const std::string &start) {
            start_ = start;
        };

        void SetEnd(const std::string &end) {
            end_ = end;
        };

        void SetColor(const std::string &color) {
            color_ = color;
        };

        const std::string &GetStart() const {
            return start_;
        };

        const std::string &GetEnd() const {
            return end_;
        };

        const std::string &GetColor() const {
            return color_;
        };
};

class Node {
    private:
        std::string name_;
        std::string label_;
        std::string color_;
        std::string border_color_;

    public:
        Node()
            :name_(), label_(), color_(), border_color_() {};

        virtual ~Node() = default;

        void SetName(const std::string &name) {
            name_ = name;
        };

        void SetLabel(const std::string &label) {
            label_ = label;
        };

        void SetColor(const std::string &color) {
            color_ = color;
        };

        void SetBorderColor(const std::string &color) {
            border_color_ = color;
        };

        const std::string &GetName() const {
            return name_;
        };

        const std::string &GetLabel() const {
            return label_;
        };

        const std::string &GetColor() const {
            return color_;
        };

        const std::string &GetBorderColor() const {
            return border_color_;
        };
};

class Cluster : virtual public Node {
    private:
        std::vector<Cluster*> sub_clusters_;

        std::vector<Node*> nodes_;

        std::vector<Edge> edges_;

        size_t entrance_;

    public:
        Cluster()
            :Node(), sub_clusters_(), nodes_(), edges_(), entrance_(0) {};

        ~Cluster() {
            for (auto *child : sub_clusters_) {
                delete child;
            }
            for (auto *child : nodes_) {
                delete child;
            }
        };

        void AddChild(Node *child) {
            nodes_.push_back(child);
        };
        void AddChild(Cluster *child) {
            sub_clusters_.push_back(child);
        };

        void AddEdge(const Edge edge) {
            edges_.push_back(edge);
        };

        Cluster *GetSubCluster(size_t idx) const {
            if (idx >= sub_clusters_.size()) {
                return nullptr;
            }
            return sub_clusters_[idx];
        };
        Node *GetNode(size_t idx) const {
            if (idx >= nodes_.size()) {
                return nullptr;
            }
            return nodes_[idx];
        };

        const std::vector<Cluster*> &GetSubClusters() const {
            return sub_clusters_;
        };
        const std::vector<Node*> &GetNodes() const {
            return nodes_;
        };

        const std::vector<Edge> &GetEdges() const {
            return edges_;
        };

        size_t GetEntrance() const {
            return entrance_;
        };

        void IncrementEntrance() {
            entrance_++;
        };
};

}; // namespace graph
