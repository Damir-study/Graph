#include "../../include/Graph/Graph.hpp"

#include <stdexcept>

template <typename V, typename W>
Graph<V, W>::Graph(Storage* storage)
    : adjacency(storage),
      vertex_list(),
      edge_count(0) {}

template <typename V, typename W>
bool Graph<V, W>::add_vertex(const V& vertex) {
    if (contains_vertex(vertex)) return false;

    adjacency.add(vertex, NeighborList());
    vertex_list.append(vertex);
    return true;
}

template <typename V, typename W>
bool Graph<V, W>::remove_vertex(const V& vertex) {
    if (!contains_vertex(vertex)) return false;

    for (int i = 0; i < vertex_list.get_length(); ++i) {
        const V& u = vertex_list[i];
        if (contains_edge(u, vertex)) {
            remove_edge(u, vertex);
        }
        if (contains_edge(vertex, u)) {
            remove_edge(vertex, u);
        }
    }

    adjacency.remove(vertex);

    mutable_array_sequence<V> new_vertices;
    for (int i = 0; i < vertex_list.get_length(); ++i) {
        if (!(vertex_list[i] == vertex)) {
            new_vertices.append(vertex_list[i]);
        }
    }
    vertex_list = new_vertices;

    return true;
}

template <typename V, typename W>
bool Graph<V, W>::contains_vertex(const V& vertex) const {
    return adjacency.contains_key(vertex);
}

template <typename V, typename W>
bool Graph<V, W>::contains_edge(const V& from, const V& to) const {
    if (!contains_vertex(from) || !contains_vertex(to)) return false;

    const NeighborList& neighbors = adjacency.get(from);
    for (int i = 0; i < neighbors.get_length(); ++i) {
        if (neighbors[i].vertex == to) {
            return true;
        }
    }

    return false;
}

template <typename V, typename W>
const W& Graph<V, W>::get_edge_weight(const V& from, const V& to) const {
    if (!contains_vertex(from)) {
        throw std::invalid_argument("Исходная вершина не найдена в графе");
    }
    if (!contains_vertex(to)) {
        throw std::invalid_argument("Целевая вершина не найдена в графе");
    }

    const NeighborList& neighbors = adjacency.get(from);
    for (int i = 0; i < neighbors.get_length(); ++i) {
        if (neighbors[i].vertex == to) {
            return neighbors[i].weight;
        }
    }

    throw std::invalid_argument("Ребро между указанными вершинами отсутствует");
}

template <typename V, typename W>
const mutable_array_sequence<V>& Graph<V, W>::get_vertices() const {
    return vertex_list;
}

template <typename V, typename W>
std::size_t Graph<V, W>::get_vertex_count() const {
    return static_cast<std::size_t>(vertex_list.get_length());
}

template <typename V, typename W>
std::size_t Graph<V, W>::get_edge_count() const {
    return edge_count;
}

template <typename V, typename W>
bool Graph<V, W>::is_empty() const {
    return vertex_list.get_length() == 0;
}

template <typename V, typename W>
void Graph<V, W>::clear() {
    adjacency.clear();
    vertex_list = mutable_array_sequence<V>();
    edge_count = 0;
}
