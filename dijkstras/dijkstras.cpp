#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include <random>
#include <cassert>

class Node {
public:
    explicit Node(size_t id)
        : id(id), visited(false), distance_to_S(std::numeric_limits<size_t>::max()) {}

    size_t id;
    bool visited;
    std::vector<std::pair<size_t, Node *>> neighbors; // distance, node
    size_t distance_to_S;

    void print_node() const {
        std::cout << "Node " << id << " has distance " <<  (distance_to_S == std::numeric_limits<size_t>::max() ? "infinity" : std::to_string(distance_to_S)) << "\n  ";
        for (auto [distance, neighbor] : neighbors) {
            std::cout << "Node " << neighbor->id << " is " << distance << " away\n  ";
        }
        std::cout << std::endl;
    }
};

class PriorityStructure { // binary heap vs fibonacci heap vs set
public:
    virtual ~PriorityStructure() = default;
    virtual Node *pop_min() = 0;
    virtual void insert(Node *node) = 0;
    virtual void decrease_key(Node *node, size_t new_val) = 0;
    virtual bool is_empty() const = 0;
    virtual void print_heap() = 0;
};

class Set : public PriorityStructure {
public:
    struct SetComparator {
        bool operator()(const Node *a, const Node *b) const {
            if (a->distance_to_S != b->distance_to_S) {
                return a->distance_to_S < b->distance_to_S;
            }
            return a->id < b->id;
        }
    };

    std::set<Node *, SetComparator> node_set;

    Node *pop_min() override {
        Node *min_node = *node_set.begin();
        node_set.erase(node_set.begin());
        return min_node;
    }

    void insert(Node *node) override {
        node_set.insert(node);
    }

    void decrease_key(Node *node, size_t new_val) override {
        node_set.erase(node);
        node->distance_to_S = new_val;
        node_set.insert(node);
    }

    bool is_empty() const override { return node_set.empty(); }

    void print_heap() override {
        for (auto *node : node_set) {
            std::cout << "Node " << node->id << " has distance " << node->distance_to_S << "\n  ";
        }
    }
};

class DFSDeque : public PriorityStructure {
public:
    std::deque<Node *> node_deque;
    Node *pop_min() override {
        Node *node = node_deque.front();
        node_deque.pop_front();
        return node;
    }
    void insert(Node *node) override {
        node_deque.push_back(node);
    }
    void decrease_key(Node *node, size_t new_val) override {
        for (auto &n : node_deque) {
            if (n->id == node->id) {
                n->distance_to_S = new_val;
                break;
            }
        }
    }
    bool is_empty() const override {
        return node_deque.empty();
    }
    void print_heap() override {
    }
};

class Graph {
public:
    std::vector<Node *> nodes;
    std::unique_ptr<PriorityStructure> heap;
    size_t total_distance = 0;
    size_t num_nodes_visited = 0;

    Graph(size_t num_nodes, double edge_density, std::string heap_type) {
        for (size_t i = 0; i < num_nodes; i++) {
            nodes.push_back(new Node(i));
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        std::uniform_int_distribution<size_t> weight_dis(0, 9);

        for (size_t i = 0; i < num_nodes; i++) {
            for (size_t j = 0; j < num_nodes; j++) {
                if (i == j) {
                    continue;
                }
                size_t weight = weight_dis(gen);
                if (dis(gen) < edge_density) {
                    nodes[i]->neighbors.push_back(std::make_pair(weight, nodes[j]));
                }
            }

        }
        // for (auto *node : nodes) {
        //     node->print_node();
        // }
        if (heap_type == "set") {
            heap = std::make_unique<Set>();
        } else if (heap_type == "deque") {
            heap = std::unique_ptr<PriorityStructure>(new DFSDeque());
        } else {
            throw std::invalid_argument("Invalid heap type");
        }
        nodes[0]->distance_to_S = 0;
        heap->insert(nodes[0]);
    }

    std::pair<size_t, size_t> run_dijkstras() {
        while (!heap->is_empty()) {
            run_one_iter();
        }
        return std::make_pair(total_distance, num_nodes_visited);
    }

    void run_one_iter() {

        Node *closest_node = nullptr;
        while ((!closest_node || closest_node->visited) && !heap->is_empty()) {
            closest_node = heap->pop_min();
        }
        total_distance += closest_node->distance_to_S;
        num_nodes_visited++;
        closest_node->visited = true;

        for (auto [distance, neighbor] : closest_node->neighbors) {
            if (neighbor->visited) {
               continue;
            }
            if (neighbor->distance_to_S == std::numeric_limits<size_t>::max()) {
                neighbor->distance_to_S = distance;
                heap->insert(neighbor);
            }            
            else if (distance < neighbor->distance_to_S) {
                heap->decrease_key(neighbor, distance);
            }
        }
        // this->heap->print_heap();
    }

    ~Graph() {
        for (auto *node : nodes) {
            delete node;
        }
    }
};

int main() {
    size_t num_nodes = 7;
    double edge_density = 0.3;
    Graph graph(num_nodes, edge_density,"deque"); // eventually makea  copy of the graph
    // and test with each heap type
    auto total_distance = graph.run_dijkstras();
    std::cout << "Total distance: " << total_distance.first << std::endl;
    std::cout << "Number of nodes visited: " << total_distance.second << std::endl;

    // i can compare directed to undirected, use different edge densities, etc.
    // can also find distances using BFS, DFS, etc.

    return 0;
}
