#include "../../include/Graph/UndirectedGraph.hpp"

template <typename V, typename W>
UndirectedGraph<V, W>::UndirectedGraph()
    : Graph<V, W>(new BinaryTreeDictionaryStorage<V, typename Graph<V, W>::NeighborList>(
          new BalancedTree<DictionaryPair<V, typename Graph<V, W>::NeighborList>>())) {}

template <typename V, typename W>
UndirectedGraph<V, W>::UndirectedGraph(Storage* storage)
    : Graph<V, W>(storage) {}

template <typename V, typename W>
bool UndirectedGraph<V, W>::add_edge(const V& from, const V& to, const W& weight) {
    if (!this->contains_vertex(from) || !this->contains_vertex(to)) return false;
    if (this->contains_edge(from, to)) return false;

    Neighbor neighbor_to{to, weight};
    this->adjacency.get(from).append(neighbor_to);

    if (!(from == to)) {
        Neighbor neighbor_from{from, weight};
        this->adjacency.get(to).append(neighbor_from);
    }

    this->edge_count++;
    return true;
}

template <typename V, typename W>
bool UndirectedGraph<V, W>::remove_edge(const V& from, const V& to) {
    if (!this->contains_vertex(from) || !this->contains_vertex(to)) return false;
    if (!this->contains_edge(from, to)) return false;

    NeighborList& from_neighbors = this->adjacency.get(from);
    NeighborList new_from_neighbors;
    bool found_from = false;
    for (int i = 0; i < from_neighbors.get_length(); ++i) {
        if (!found_from && from_neighbors[i].vertex == to) {
            found_from = true;
            continue;
        }
        new_from_neighbors.append(from_neighbors[i]);
    }
    from_neighbors = new_from_neighbors;

    if (!(from == to)) {
        NeighborList& to_neighbors = this->adjacency.get(to);
        NeighborList new_to_neighbors;
        bool found_to = false;
        for (int i = 0; i < to_neighbors.get_length(); ++i) {
            if (!found_to && to_neighbors[i].vertex == from) {
                found_to = true;
                continue;
            }
            new_to_neighbors.append(to_neighbors[i]);
        }
        to_neighbors = new_to_neighbors;
    }

    this->edge_count--;
    return true;
}

template <typename V, typename W>
bool UndirectedGraph<V, W>::is_directed() const {
    return false;
}
