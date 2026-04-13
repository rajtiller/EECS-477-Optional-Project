#include <vector>
#include <iostream>


class Node {
    public:
    Node(int id, int val) : val(val), id(id), visited(false), distance_to_source(0) {}
    int val;
    int id;
    bool visited;
    std::vector<std::pair<int, Node *>> neighbors; // distance, node
    int distance_to_source;
};


class Heap { // try making this a binary heap vs a fibonacci heap vs std::priority_queue
    public:
    Heap(int num_nodes) : num_nodes(num_nodes) {}
    int num_nodes;
    Node * find_min();
    void insert(int val);
    void decrease_key(int val, int new_val);
    void delete_min();
};

class Graph {
    public:
    Graph(int num_nodes) : num_nodes(num_nodes), heap(num_nodes) {}
    int num_nodes;
    Heap heap;
};

void run_one_iter(Graph &graph) {
    Node * closest_node = graph.heap.find_min();
    graph.heap.delete_min();
    closest_node->visited = true;
    for (auto [distance, neighbor] : closest_node->neighbors) {
        int new_distance = closest_node->distance_to_source + distance;
        if (new_distance < neighbor->distance_to_source) {
            neighbor->distance_to_source = new_distance;
            if (!neighbor->visited || new_distance < neighbor->distance_to_source) {
            graph.heap.decrease_key(neighbor->id, new_distance);
        }
    }
    }
}

int main() {

    size_t num_nodes = 10;
    Graph graph(num_nodes);
    size_t num_edges = num_nodes * 2;
    // randomly generate num_edges edges
    for (size_t i = 0; i < num_edges; i++) {
        int u = rand() % num_nodes;
        int v = rand() % num_nodes;
        int weight = rand() % 10;
        graph.add_edge(u, v, weight); // fix this tmw
    }

    for (int v = 0; v < 10; ++v) {
        graph.heap.insert(v);
    }
    return 0;
}

Node *Heap::find_min() { return nullptr; }

void Heap::insert(int val) { (void)val; }

void Heap::decrease_key(int val, int new_val) {
    (void)val;
    (void)new_val;
}

void Heap::delete_min() {}