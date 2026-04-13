#include <vector>
#include <iostream>
#include <set>


class Node {
    public:
    Node(size_t id) : id(id), visited(false), distance_to_source(std::numeric_limits<size_t>::max()) {}
    size_t id;
    bool visited;
    std::vector<std::pair<size_t, Node *>> neighbors; // distance, node
    size_t distance_to_source;
    void print_node() {
        std::cout << "Node " << id << " has distance " << distance_to_source << " and is " << (visited ? "visited" : "not visited") << std::endl;
        for (auto [distance, neighbor] : neighbors) {
            std::cout << "Neighbor " << neighbor->id << " has distance " << distance << std::endl;
        }
    }
};


class PriorityStructure { // binary heap vs fibonacci heap vs set
    public:
    PriorityStructure() {}
    Node * pop_min();
    void insert(Node * node, size_t distance);
    void decrease_key(Node *, size_t new_val);
    bool is_empty();
};

class Set : public PriorityStructure {
    public:
    class SetComparator {
        public:
        bool operator()(Node * a, Node * b) {
            return a->distance_to_source > b->distance_to_source;
        }
    };
    std::set<Node *, SetComparator> node_set;
    Set(Node * source) : PriorityStructure() {
        for (auto [distance, neighbor] : source->neighbors) {
            neighbor->distance_to_source = distance;
            node_set.insert(neighbor);
    }}
    
    Node * pop_min() {
        Node * min_node = *node_set.begin();
        node_set.erase(node_set.begin());
        return min_node;
    }
    void insert(Node * node, size_t distance) {
        node->distance_to_source = distance;
        node_set.insert(node);
    }
    void decrease_key(Node * node, size_t new_val) {
        node_set.erase(node);
        node->distance_to_source = new_val;
        node_set.insert(node);
    }

    bool is_empty() {
        return node_set.empty();
    }
};

class Graph {
    public:
    std::vector<Node*> nodes;
    PriorityStructure heap;
    Graph(size_t num_nodes, size_t num_edges) {
        for (size_t i = 0; i < num_nodes; i++) {
            nodes.push_back(new Node(i));
        }
        // randomly generate num_edges edges
        for (size_t i = 0; i < num_edges; i++) {
            size_t u = rand() % num_nodes;
            size_t v = rand() % num_nodes;
            size_t weight = rand() % 10;
            if (u == v) continue;
            this->nodes[u]->neighbors.push_back(std::make_pair(weight, this->nodes[v])); // fix this tmw
        }
        heap = Set(nodes[0]);
    }

    void run_dijkstras(Graph &graph) {
        while (!graph.heap.is_empty()) {
            run_one_iter(graph);
        }
    }

    void run_one_iter(Graph &graph) {
        Node * closest_node = graph.heap.pop_min();
        closest_node->visited = true;
        for (auto [distance, neighbor] : closest_node->neighbors) {
            size_t new_distance = closest_node->distance_to_source + distance;
            if (new_distance < neighbor->distance_to_source) {
                if (!neighbor->visited || new_distance < neighbor->distance_to_source) {
                graph.heap.decrease_key(neighbor, new_distance);
            }
        }
        }
    }


    ~Graph() {
        for (auto &node : nodes) {
            delete node;
        }
    }
};



int main() {

    size_t num_nodes = 10;
    size_t num_edges = 20;
    Graph graph(num_nodes, num_edges);
    for (size_t i = 0; i < num_nodes; i++) {
        graph.nodes[i]->print_node();
    }

    graph.run_dijkstras(graph);
    for (size_t i = 0; i < num_nodes; i++) {
        graph.nodes[i]->print_node();
    }

    
    return 0;
}
