#include "../../include/Graph/DirectedGraph.hpp"

template <typename V, typename W>
DirectedGraph<V, W>::DirectedGraph()
    : Graph<V, W>(new BinaryTreeDictionaryStorage<V, typename Graph<V, W>::NeighborList>(
          new BalancedTree<DictionaryPair<V, typename Graph<V, W>::NeighborList>>())) {}

template <typename V, typename W>
DirectedGraph<V, W>::DirectedGraph(Storage* storage)
    : Graph<V, W>(storage) {}

template <typename V, typename W>
bool DirectedGraph<V, W>::add_edge(const V& from, const V& to, const W& weight) {
    if (!this->contains_vertex(from) || !this->contains_vertex(to)) return false;
    if (this->contains_edge(from, to)) return false;

    Neighbor neighbor{to, weight};
    this->adjacency.get(from).append(neighbor);
    this->edge_count++;
    return true;
}

template <typename V, typename W>
bool DirectedGraph<V, W>::remove_edge(const V& from, const V& to) {
    if (!this->contains_vertex(from) || !this->contains_vertex(to)) return false;

    NeighborList& neighbors = this->adjacency.get(from);
    NeighborList new_neighbors;
    bool found = false;

    for (int i = 0; i < neighbors.get_length(); ++i) {
        if (!found && neighbors[i].vertex == to) {
            found = true;
            continue;
        }
        new_neighbors.append(neighbors[i]);
    }

    if (!found) return false;

    neighbors = new_neighbors;
    this->edge_count--;
    return true;
}

template <typename V, typename W>
bool DirectedGraph<V, W>::is_directed() const {
    return true;
}
