#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <vector>

class Node {
  public:
    explicit Node(size_t id)
        : id(id), visited(false), distance_to_S(std::numeric_limits<size_t>::max()),
          parent(nullptr), rank(0), marked(false) {}

    size_t id;
    bool visited;
    std::vector<std::pair<size_t, Node *>> neighbors; // distance, node
    size_t distance_to_S;
    Node *parent;                 // for fibonacci heap
    std::vector<Node *> children; // for fibonacci heap
    int rank;                     // for fibonacci heap
    bool marked;                  // for fibonacci heap

    void print_node() const {
        std::cout << "Node " << id << " has distance "
                  << (distance_to_S == std::numeric_limits<size_t>::max()
                          ? "infinity"
                          : std::to_string(distance_to_S))
                  << "\n  ";
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

    void insert(Node *node) override { node_set.insert(node); }

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
                return a->distance_to_S > b->distance_to_S;
            }
            return a->id > b->id;
        }
    };

    std::priority_queue<Node *, std::vector<Node *>, SetComparator> pq;

    Node *pop_min() override {
        Node *min_node = pq.top();
        pq.pop();
        return min_node;
    }

    void insert(Node *node) override { pq.push(node); }

    void decrease_key(Node *node, size_t new_val) override {
        node->distance_to_S = new_val;
        pq.push(node);
    }

    bool is_empty() const override { return pq.empty(); }

    void print_heap() override {
        // auto new_pq = pq;
        // while (!new_pq.empty()) {
        //   std::cout << "Node " << new_pq.top()->id << " has distance "
        //             << new_pq.top()->distance_to_S << "\n  ";
        //   new_pq.pop();
        // }
    }

    void print_heap(std::string message) {
        // std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
        // std::cout << message << std::endl;
        // std::cout << "priority queue size: " << pq.size() << std::endl;
        // auto new_pq = pq;
        // while (!new_pq.empty()) {
        //   std::cout << "Node " << new_pq.top()->id << " has distance "
        //             << new_pq.top()->distance_to_S << "\n  ";
        //   new_pq.pop();
        // }
        // std::cout << "*************************\n";
    }
};

class FibonacciHeap : public PriorityStructure {
  public:
    std::vector<Node *> roots;
    int max_rank = -1; // -1 because we start at 0 and increment
    std::string name() const override { return "fibonacci heap"; }
    struct FibonacciHeapComparator {
        bool operator()(const Node *a, const Node *b) const {
            if (a->distance_to_S != b->distance_to_S) {
                return a->distance_to_S < b->distance_to_S;
            }
            return a->id < b->id;
        }
    };

    Node *Merge(Node *a, Node *b) {
        if (!(a && b && a->rank == b->rank)) {
            std::cout << "Merge error: a: " << a->id << " b: " << b->id << " a->rank: " << a->rank
                      << " b->rank: " << b->rank << std::endl;
        }
        assert(a && b && a->rank == b->rank);
        if (a->distance_to_S < b->distance_to_S) {
            a->children.push_back(b);
            b->parent = a;
            a->rank++;
            return a;
        } else {
            b->children.push_back(a);
            a->parent = b;
            b->rank++;
            return b;
        }
    }

    Node *pop_min() override {
        print_heap("before popping min node");
        size_t min_distance = std::numeric_limits<size_t>::max();
        Node *min_node = nullptr;
        for (auto *root : roots) {
            if (root && root->distance_to_S < min_distance) {
                min_distance = root->distance_to_S;
                min_node = root;
            }
        }
        roots[min_node->rank] = nullptr;
        if (min_node->rank == max_rank) {
            assert(max_rank == static_cast<int>(roots.size()) - 1);
            int new_max_rank = max_rank;
            while (new_max_rank >= 0 && roots[new_max_rank] == nullptr) {
                new_max_rank--;
                roots.pop_back();
            }
            max_rank = new_max_rank;
        }
        std::vector<Node *> new_roots = min_node->children;
        min_node->children.clear();
        min_node->marked = false;
        min_node->rank = 0;
        min_node->parent = nullptr;
        for (size_t i = 0; i < new_roots.size(); i++) {
            new_roots[i]->parent = nullptr;
            insert(new_roots[i]);
        }
        print_heap("after popping min node");
        return min_node;
    }

    void insert(Node *node) override {
        // node->parent = nullptr; // removing these so we can use insert in decrease_key
        // node->marked = false;
        // node->children.clear();
        // node->rank = 0;
        print_heap("before inserting node " + std::to_string(node->id) +
                   " rank: " + std::to_string(node->rank));
        Node *carry = node;
        while (true) {
            assert(carry);
            if (carry->rank > max_rank) {
                roots.resize(carry->rank + 1, nullptr); // i hope this works
                max_rank = carry->rank;
            }
            if (roots[carry->rank] == nullptr) {
                roots[carry->rank] = carry;
                break;
            }
            Node *temp = roots[carry->rank];
            roots[carry->rank] = nullptr;
            carry = Merge(temp, carry); // this should update the rank of carry
        }
        print_heap("after inserting node " + std::to_string(node->id));
    }

    void decrease_key(Node *node, size_t new_val) override {
        std::string node_id = std::to_string(node->id);
        print_heap("before decreasing key of node " + node_id);
        assert(node->visited == false);
        node->distance_to_S = new_val;

        std::stack<Node *> removed_nodes;
        Node *node_to_be_removed = node;
        removed_nodes.push(node_to_be_removed);
        while (true) {
            // adjust current node then update it if it's parent is marked
            if (node_to_be_removed != node) {
                node_to_be_removed->rank -= 2; // i think this is correct but we need to test it
                assert(node_to_be_removed->rank >= 0);
            }
            if (node_to_be_removed->parent) {
                node_to_be_removed->parent->children.erase(
                    std::find(node_to_be_removed->parent->children.begin(),
                              node_to_be_removed->parent->children.end(), node_to_be_removed));
                // make sure above line works
                if (node_to_be_removed->parent->marked) {
                    node_to_be_removed = node_to_be_removed->parent;
                    continue;
                }
            }
            break;
        }
        // at this point node_to_be_removed is the last removed node
        if (node_to_be_removed->parent) {
            assert(node_to_be_removed->parent->marked == false);
            node_to_be_removed->parent->marked = true;
        } else {
            size_t former_rank = node_to_be_removed->rank + 2;
            if (node_to_be_removed == node) {
                former_rank = node_to_be_removed->rank;
            }
            if (roots[former_rank] != node_to_be_removed) {
                std::cout << "expected id: " << node_to_be_removed->id
                          << " but got id: " << roots[former_rank]->id << std::endl;
            }
            assert(roots[former_rank] == node_to_be_removed);
            // +2 above because we subtracted 2 from the rank when we removed the node
            roots[former_rank] = nullptr;
            int new_max_rank = max_rank;
            assert(max_rank == static_cast<int>(roots.size()) - 1);
            while (new_max_rank >= 0 && roots[new_max_rank] == nullptr) {
                new_max_rank--;
                roots.pop_back();
            }
            max_rank = new_max_rank;
        }

        while (!removed_nodes.empty()) {
            Node *removed_node = removed_nodes.top();
            removed_nodes.pop();
            removed_node->parent = nullptr;
            removed_node->marked = false;
            insert(removed_node);
        }
        print_heap("after decreasing key of node " + node_id);
    }

    bool is_empty() const override { return roots.empty(); }
    void print_heap() override {}
    void print_heap(std::string message) {
        std::cout << "*************************\n";
        std::cout << message << std::endl;
        std::cout << "max_rank: " << max_rank << std::endl;
        std::cout << "roots size: " << roots.size() << std::endl;
        for (auto *root : roots) {
            if (!root) {
                continue;
            }
            std::stack<Node *> nodes_to_print;
            nodes_to_print.push(root);
            while (!nodes_to_print.empty()) {
                Node *node = nodes_to_print.top();
                nodes_to_print.pop();
                std::cout << "Node " << node->id << (node->marked ? " (X)" : "") << " has distance "
                          << node->distance_to_S << " rank: " << node->rank << "\n  ";
                for (auto it = node->children.rbegin(); it != node->children.rend(); ++it) {
                    Node *child = *it;
                    nodes_to_print.push(child);
                }
            }
            std::cout << "\n";
        }
        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
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
    void insert(Node *node) override { node_deque.push_front(node); }
    void decrease_key(Node *node, size_t new_val) override {
        for (auto &n : node_deque) {
            if (n->id == node->id) {
                n->distance_to_S = new_val;
                break;
            }
        }
    }
    bool is_empty() const override { return node_deque.empty(); }
    void print_heap() override {}
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
    void insert(Node *node) override { node_deque.push_back(node); }
    void decrease_key(Node *node, size_t new_val) override {
        for (auto &n : node_deque) {
            if (n->id == node->id) {
                n->distance_to_S = new_val;
                break;
            }
        }
    }
    bool is_empty() const override { return node_deque.empty(); }
    void print_heap() override {}
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
        // std::cout << "potential closest node: " << closest_node->id
        //           << "address: " << closest_node << std::endl;
    }
    if (closest_node->visited) {
        return;
    }
    std::cout << "closest_node: " << closest_node->id
              << "---distance: " << closest_node->distance_to_S << std::endl;
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
        } else if (distance < neighbor->distance_to_S) {
            heap->decrease_key(neighbor, distance);
        }
    }
}

std::pair<size_t, size_t> run_dijkstras(Graph &graph, PriorityStructure *heap) {
    std::cout << "running dijkstras with heap: " << heap->name() << std::endl;
    graph.nodes[0]->distance_to_S = 0;
    heap->insert(graph.nodes[0]);
    while (!heap->is_empty()) {
        run_one_iter(graph, heap);
    }
    std::cout << "--------------------------------" << std::endl;
    return std::make_pair(graph.total_distance, graph.num_nodes_visited);
}

std::vector<std::pair<size_t, std::pair<size_t, size_t>>> generate_edges(size_t num_nodes,
                                                                         double edge_density) {
    std::vector<std::pair<size_t, std::pair<size_t, size_t>>> edges;
    for (size_t i = 0; i < num_nodes; i++) {
        for (size_t j = 0; j < num_nodes; j++) {
            if (i == j) {
                continue;
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            size_t weight = std::uniform_int_distribution<size_t>(0, 100)(gen);
            if (std::uniform_real_distribution<double>(0.0, 1.0)(gen) < edge_density) {
                edges.push_back(std::make_pair(i, std::make_pair(weight, j)));
            }
        }
    }
    return edges;
}

int main() {

    std::ofstream file("dijkstras_running_times.csv");
    file << "category,num_nodes,edge_density,seconds,total_distance,num_nodes_"
            "visited"
         << std::endl;

    //   size_t num_nodes = 7;
    //   double edge_density = 0.3;
    for (double edge_density = 0.1; edge_density <= 0.9; edge_density += 0.1) {
        for (size_t num_nodes = 10; num_nodes <= 1000000; num_nodes *= 2) {
            auto edges = generate_edges(num_nodes, edge_density);
            Graph graph_1(num_nodes, edges);
            for (auto *node : graph_1.nodes) {
                node->print_node();
            }
            // and test with each heap type
            std::vector<PriorityStructure *> heaps = {new Set(), new PriorityQueue(),
                                                      new FibonacciHeap(), new DFSDeque(),
                                                      new BFSDeque()};
            for (auto *heap : heaps) {
                Graph graph(num_nodes, edges);
                auto start = std::chrono::steady_clock::now();
                auto total_distance = run_dijkstras(graph, heap);
                auto end = std::chrono::steady_clock::now();
                double seconds = std::chrono::duration<double>(end - start).count();
                file << heap->name() << "," << num_nodes << "," << edge_density << "," << seconds
                     << "," << total_distance.first << "," << total_distance.second << std::endl;
            }
        }
    }

    // mess around with fibonacci heap a bit

    return 0;
}
