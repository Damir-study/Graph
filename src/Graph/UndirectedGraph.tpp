#ifndef UNDIRECTED_GRAPH_TPP
#define UNDIRECTED_GRAPH_TPP

#include "../../include/Graph/UndirectedGraph.hpp"

#include <expected>
#include <string>

template <typename V, typename W>
UndirectedGraph<V, W>::UndirectedGraph()
    : Graph<V, W>(new BinaryTreeDictionaryStorage<V, NeighborList>(
          new BalancedTree<DictionaryPair<V, NeighborList>>())) {}

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
bool UndirectedGraph<V, W>::set_edge_weight(const V& from, const V& to, const W& weight) {
    if (!this->contains_vertex(from) || !this->contains_vertex(to)) return false;
    if (!this->contains_edge(from, to)) return false;

    NeighborList& from_neighbors = this->adjacency.get(from);
    for (int i = 0; i < from_neighbors.get_length(); ++i) {
        if (from_neighbors[i].vertex == to) {
            from_neighbors[i].weight = weight;
            break;
        }
    }

    if (!(from == to)) {
        NeighborList& to_neighbors = this->adjacency.get(to);
        for (int i = 0; i < to_neighbors.get_length(); ++i) {
            if (to_neighbors[i].vertex == from) {
                to_neighbors[i].weight = weight;
                break;
            }
        }
    }

    return true;
}

template <typename V, typename W>
bool UndirectedGraph<V, W>::is_directed() const {
    return false;
}

template <typename V, typename W>
std::expected<std::size_t, std::string> UndirectedGraph<V, W>::get_degree(const V& vertex) const {
    if (!this->contains_vertex(vertex)) {
        return std::unexpected("Вершина не найдена в графе");
    }
    return static_cast<std::size_t>(this->adjacency.get(vertex).get_length());
}

template <typename V, typename W>
bool UndirectedGraph<V, W>::has_cycle() const {
    Dictionary<V, bool> visited(
        new BinaryTreeDictionaryStorage<V, bool>(
            new BalancedTree<DictionaryPair<V, bool>>()));
    Dictionary<V, V> parent(
        new BinaryTreeDictionaryStorage<V, V>(
            new BalancedTree<DictionaryPair<V, V>>()));

    const mutable_array_sequence<V>& vertices = this->get_vertices();

    for (int i = 0; i < vertices.get_length(); ++i) {
        const V& root = vertices[i];
        if (visited.contains_key(root)) {
            continue;
        }

        mutable_array_sequence<V> queue;
        visited.add(root, true);
        queue.append(root);

        int head = 0;
        while (head < queue.get_length()) {
            V u = queue[head++];
            const NeighborList& neighbors = this->adjacency.get(u);
            for (int j = 0; j < neighbors.get_length(); ++j) {
                const V& v = neighbors[j].vertex;
                if (v == u) {
                    return true; // Петля
                }
                if (!visited.contains_key(v)) {
                    visited.add(v, true);
                    parent.add(v, u);
                    queue.append(v);
                } else {
                    // Вершина v уже посещена. Если это не родитель вершины u, значит есть цикл.
                    if (!parent.contains_key(u) || !(parent.get(u) == v)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

template <typename V, typename W>
bool UndirectedGraph<V, W>::is_connected() const {
    if (this->get_vertex_count() <= 1) return true;
    const mutable_array_sequence<V>& vertices = this->get_vertices();
    std::expected<mutable_array_sequence<V>, std::string> dfs_res = this->dfs(vertices[0]);
    if (!dfs_res) return false;
    return dfs_res->get_length() == static_cast<int>(this->get_vertex_count());
}

template <typename V, typename W>
bool UndirectedGraph<V, W>::is_tree() const {
    if (this->get_vertex_count() <= 1) return true;
    return is_connected() && !has_cycle();
}

template <typename V, typename W>
UndirectedGraph<V, W>* UndirectedGraph<V, W>::get_subgraph(const mutable_array_sequence<V>& vertices) const {
    UndirectedGraph<V, W>* sub = new UndirectedGraph<V, W>();

    Dictionary<V, bool> in_sub(
        new BinaryTreeDictionaryStorage<V, bool>(
            new BalancedTree<DictionaryPair<V, bool>>()));

    for (int i = 0; i < vertices.get_length(); ++i) {
        const V& v = vertices[i];
        if (this->contains_vertex(v) && !in_sub.contains_key(v)) {
            in_sub.add(v, true);
            sub->add_vertex(v);
        }
    }

    const mutable_array_sequence<V>& sub_vertices = sub->get_vertices();
    for (int i = 0; i < sub_vertices.get_length(); ++i) {
        const V& u = sub_vertices[i];
        const NeighborList& neighbors = this->adjacency.get(u);
        for (int j = 0; j < neighbors.get_length(); ++j) {
            const V& v = neighbors[j].vertex;
            if (in_sub.contains_key(v) && !sub->contains_edge(u, v)) {
                sub->add_edge(u, v, neighbors[j].weight);
            }
        }
    }

    return sub;
}

#endif // UNDIRECTED_GRAPH_TPP
