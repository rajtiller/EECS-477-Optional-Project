#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include <random>
#include <cassert>
#include <fstream>

class Node {
public:
    explicit Node(size_t id)
        : id(id), visited(false), distance_to_S(std::numeric_limits<size_t>::max()), marked(false) {}

    size_t id;
    bool visited;
    std::vector<std::pair<size_t, Node *>> neighbors; // distance, node
    size_t distance_to_S;
    std::vector<Node *> children; // for fibonacci heap
    bool marked; // for fibonacci heap

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
    virtual std::string name() const = 0;
    virtual Node *pop_min() = 0;
    virtual void insert(Node *node) = 0;
    virtual void decrease_key(Node *node, size_t new_val) = 0;
    virtual bool is_empty() const = 0;
    virtual void print_heap() = 0;
};

class Set : public PriorityStructure {
public:
    std::string name() const override { return "set"; }
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

class PriorityQueue : public PriorityStructure {
    public:
        std::string name() const override { return "priority queue"; }
        struct SetComparator {
            bool operator()(const Node *a, const Node *b) const {
                if (a->distance_to_S != b->distance_to_S) {
                    return a->distance_to_S < b->distance_to_S;
                }
                return a->id < b->id;
            }
        };
    
        std::priority_queue<Node *, std::vector<Node *>, SetComparator> pq;
    
        Node *pop_min() override {
            Node *min_node = pq.top();
            pq.pop();
            return min_node;
        }
    
        void insert(Node *node) override {
            pq.push(node);
        }
    
        void decrease_key(Node *node, size_t new_val) override {
            pq.push(node);
        }
    
        bool is_empty() const override { return pq.empty(); }
    
        void print_heap() override {
            auto new_pq = pq;
            while (!new_pq.empty()) {
                std::cout << "Node " << new_pq.top()->id << " has distance " << new_pq.top()->distance_to_S << "\n  ";
                new_pq.pop();
            }
        }
    };

    class FibonacciHeap : public PriorityStructure {
        public:
            Node *root;
            std::string name() const override { return "fibonacci heap"; }
            struct FibonacciHeapComparator {
                bool operator()(const Node *a, const Node *b) const {
                    if (a->distance_to_S != b->distance_to_S) {
                        return a->distance_to_S < b->distance_to_S;
                    }
                    return a->id < b->id;
                }
            };
        
            Node *pop_min() override {
                Node *min_node = root;
                // FINISH THIS!!!
            }
        
            void insert(Node *node) override {
                
            }
        
            void decrease_key(Node *node, size_t new_val) override {
                
            }
        
            bool is_empty() const override {}
        
            void print_heap() override {
                
            }
        };


class DFSDeque : public PriorityStructure {
public:
    std::string name() const override { return "DFS"; }
    std::deque<Node *> node_deque;
    Node *pop_min() override {
        Node *node = node_deque.front();
        node_deque.pop_front();
        return node;
    }
    void insert(Node *node) override {
        node_deque.push_front(node);
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

class BFSDeque : public PriorityStructure {
    public:
        std::string name() const override { return "BFS"; }
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
    size_t total_distance = 0; // doesn't actually mean anything, just interesting
    size_t num_nodes_visited = 0;

    Graph(size_t num_nodes, std::vector<std::pair<size_t, std::pair<size_t, size_t>>> edges) {
        for (size_t i = 0; i < num_nodes; i++) {
            nodes.push_back(new Node(i));
        }

        for (auto [u, v] : edges) {
            nodes[u]->neighbors.push_back(std::make_pair(v.first, nodes[v.second]));
        }
    }

    ~Graph() {
        for (auto *node : nodes) {
            delete node;
        }
    }
};

void run_one_iter(Graph &graph, PriorityStructure *heap) {

    Node *closest_node = nullptr;
    while ((!closest_node || closest_node->visited) && !heap->is_empty()) {
        closest_node = heap->pop_min();
        std::cout << "potential closest node: " << closest_node->id << "address: " << closest_node << std::endl;
    }
    if (closest_node->visited) {
        return;
    }
    std::cout << "closest_node: " << closest_node->id << "---distance: " << closest_node->distance_to_S << std::endl;
    graph.total_distance += closest_node->distance_to_S;
    graph.num_nodes_visited++;
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

std::pair<size_t, size_t> run_dijkstras(Graph &graph, PriorityStructure *heap) {
    graph.nodes[0]->distance_to_S = 0;
    heap->insert(graph.nodes[0]);
    while (!heap->is_empty()) {
        run_one_iter(graph, heap);
    }
    return std::make_pair(graph.total_distance, graph.num_nodes_visited);
}

std::vector<std::pair<size_t, std::pair<size_t, size_t>>> generate_edges(size_t num_nodes, double edge_density) {
    std::vector<std::pair<size_t, std::pair<size_t, size_t>>> edges;
    for (size_t i = 0; i < num_nodes; i++) {
        for (size_t j = 0; j < num_nodes; j++) {
            if (i == j) {
                continue;
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            size_t weight = std::uniform_int_distribution<size_t>(0, 9)(gen); 
            if (std::uniform_real_distribution<double>(0.0, 1.0)(gen) < edge_density) {
                edges.push_back(std::make_pair(i, std::make_pair(weight, j)));
            }
        }
    }
    return edges;
}


int main() {

    std::ofstream file("dijkstras_running_times.csv");
    file << "category,num_nodes,edge_density,seconds,total_distance,num_nodes_visited" << std::endl;

    size_t num_nodes = 7;
    double edge_density = 0.3;
    auto edges = generate_edges(num_nodes, edge_density);
    Graph graph_1(num_nodes, edges);
    for (auto *node : graph_1.nodes) {
        node->print_node();
    }
    // and test with each heap type
    std::vector<PriorityStructure *> heaps = {new Set(), new PriorityQueue(), new DFSDeque(), new BFSDeque()};
    for (auto *heap : heaps) {
        Graph graph(num_nodes, edges);
        auto start = std::chrono::steady_clock::now();
        auto total_distance = run_dijkstras(graph, heap);
        auto end = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();
        file << heap->name() << "," << num_nodes << "," << edge_density << "," << seconds << "," << total_distance.first << "," << total_distance.second << std::endl;
    }
    

    // i can compare directed to undirected, use different edge densities, etc.
    // can also find distances using BFS, DFS, etc.

    return 0;
}
