#ifndef GRAPH_TPP
#define GRAPH_TPP

#include "../../include/Graph/Graph.hpp"
#include "../../TreeStructures/include/Dictionary/BinaryTreeDictionaryStorage.hpp"
#include "../../TreeStructures/include/BinaryTree/BalancedTree.hpp"

#include <expected>
#include <string>
#include <utility>

template <typename V, typename W>
Graph<V, W>::Graph(Storage* storage)
    : adjacency(storage),
      vertex_list(),
      edge_count(0) {}

template <typename V, typename W>
Graph<V, W>::Graph(const Graph& other)
    : adjacency(other.adjacency),
      vertex_list(other.vertex_list),
      edge_count(other.edge_count) {}

template <typename V, typename W>
Graph<V, W>& Graph<V, W>::operator=(const Graph& other) {
    if (this != &other) {
        adjacency = other.adjacency;
        vertex_list = other.vertex_list;
        edge_count = other.edge_count;
    }
    return *this;
}

template <typename V, typename W>
Graph<V, W>::Graph(Graph&& other)
    : adjacency(std::move(other.adjacency)),
      vertex_list(std::move(other.vertex_list)),
      edge_count(other.edge_count) {
    other.edge_count = 0;
}

template <typename V, typename W>
Graph<V, W>& Graph<V, W>::operator=(Graph&& other) {
    if (this != &other) {
        adjacency = std::move(other.adjacency);
        vertex_list = std::move(other.vertex_list);
        edge_count = other.edge_count;
        other.edge_count = 0;
    }
    return *this;
}

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
        if (!(u == vertex)) {
            if (contains_edge(u, vertex)) {
                remove_edge(u, vertex);
            }
            if (contains_edge(vertex, u)) {
                remove_edge(vertex, u);
            }
        }
    }

    if (contains_edge(vertex, vertex)) {
        remove_edge(vertex, vertex);
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
std::expected<W, std::string> Graph<V, W>::get_edge_weight(const V& from, const V& to) const {
    if (!contains_vertex(from)) {
        return std::unexpected("Исходная вершина не найдена в графе");
    }
    if (!contains_vertex(to)) {
        return std::unexpected("Целевая вершина не найдена в графе");
    }

    const NeighborList& neighbors = adjacency.get(from);
    for (int i = 0; i < neighbors.get_length(); ++i) {
        if (neighbors[i].vertex == to) {
            return neighbors[i].weight;
        }
    }

    return std::unexpected("Ребро между указанными вершинами отсутствует");
}

template <typename V, typename W>
const mutable_array_sequence<V>& Graph<V, W>::get_vertices() const {
    return vertex_list;
}

template <typename V, typename W>
std::expected<typename Graph<V, W>::NeighborList, std::string> Graph<V, W>::get_neighbors(const V& vertex) const {
    if (!contains_vertex(vertex)) {
        return std::unexpected("Вершина не найдена в графе");
    }
    return adjacency.get(vertex);
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
void Graph<V, W>::clear_edges() {
    for (int i = 0; i < vertex_list.get_length(); ++i) {
        adjacency.get(vertex_list[i]) = NeighborList();
    }
    edge_count = 0;
}

template <typename V, typename W>
void Graph<V, W>::clear() {
    adjacency.clear();
    vertex_list = mutable_array_sequence<V>();
    edge_count = 0;
}

template <typename V, typename W>
std::expected<mutable_array_sequence<V>, std::string> Graph<V, W>::dfs(const V& start) const {
    if (!contains_vertex(start)) {
        return std::unexpected("Стартовая вершина не найдена в графе");
    }

    mutable_array_sequence<V> result;
    Dictionary<V, bool> visited(
        new BinaryTreeDictionaryStorage<V, bool>(
            new BalancedTree<DictionaryPair<V, bool>>()));

    mutable_array_sequence<V> stack;
    stack.append(start);

    while (stack.get_length() > 0) {
        V current = stack.pop();

        if (visited.contains_key(current)) {
            continue;
        }

        visited.add(current, true);
        result.append(current);

        const NeighborList& neighbors = adjacency.get(current);
        // Добавляем в стек в обратном порядке, чтобы первый сосед исследовался первым
        for (int i = neighbors.get_length() - 1; i >= 0; --i) {
            const V& next = neighbors[i].vertex;
            if (!visited.contains_key(next)) {
                stack.append(next);
            }
        }
    }

    return result;
}

template <typename V, typename W>
std::expected<mutable_array_sequence<V>, std::string> Graph<V, W>::bfs(const V& start) const {
    if (!contains_vertex(start)) {
        return std::unexpected("Стартовая вершина не найдена в графе");
    }

    mutable_array_sequence<V> result;
    Dictionary<V, bool> visited(
        new BinaryTreeDictionaryStorage<V, bool>(
            new BalancedTree<DictionaryPair<V, bool>>()));

    mutable_array_sequence<V> queue;
    queue.append(start);

    int head = 0;
    while (head < queue.get_length()) {
        V current = queue[head++];

        if (visited.contains_key(current)) {
            continue;
        }

        visited.add(current, true);
        result.append(current);

        const NeighborList& neighbors = adjacency.get(current);
        for (int i = 0; i < neighbors.get_length(); ++i) {
            const V& next = neighbors[i].vertex;
            if (!visited.contains_key(next)) {
                queue.append(next);
            }
        }
    }

    return result;
}

template <typename V, typename W>
bool Graph<V, W>::has_path(const V& from, const V& to) const {
    if (!contains_vertex(from) || !contains_vertex(to)) return false;
    if (from == to) return true;

    mutable_array_sequence<V> queue;
    Dictionary<V, bool> visited(
        new BinaryTreeDictionaryStorage<V, bool>(
            new BalancedTree<DictionaryPair<V, bool>>()));

    visited.add(from, true);
    queue.append(from);

    int head = 0;
    while (head < queue.get_length()) {
        V current = queue[head++];
        const NeighborList& neighbors = adjacency.get(current);
        for (int i = 0; i < neighbors.get_length(); ++i) {
            const V& next = neighbors[i].vertex;
            if (next == to) return true;
            if (!visited.contains_key(next)) {
                visited.add(next, true);
                queue.append(next);
            }
        }
    }
    return false;
}

template <typename V, typename W>
std::expected<mutable_array_sequence<V>, std::string> Graph<V, W>::find_path(const V& from, const V& to) const {
    if (!contains_vertex(from)) {
        return std::unexpected("Исходная вершина не найдена в графе");
    }
    if (!contains_vertex(to)) {
        return std::unexpected("Целевая вершина не найдена в графе");
    }

    mutable_array_sequence<V> path;
    if (from == to) {
        path.append(from);
        return path;
    }

    mutable_array_sequence<V> queue;
    Dictionary<V, V> parent(
        new BinaryTreeDictionaryStorage<V, V>(
            new BalancedTree<DictionaryPair<V, V>>()));
    Dictionary<V, bool> visited(
        new BinaryTreeDictionaryStorage<V, bool>(
            new BalancedTree<DictionaryPair<V, bool>>()));

    visited.add(from, true);
    queue.append(from);

    int head = 0;
    bool found = false;

    while (head < queue.get_length() && !found) {
        V current = queue[head++];
        const NeighborList& neighbors = adjacency.get(current);
        for (int i = 0; i < neighbors.get_length(); ++i) {
            const V& next = neighbors[i].vertex;
            if (!visited.contains_key(next)) {
                visited.add(next, true);
                parent.add(next, current);
                queue.append(next);
                if (next == to) {
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found) {
        return std::unexpected("Путь между указанными вершинами отсутствует");
    }

    mutable_array_sequence<V> reverse_path;
    V curr = to;
    reverse_path.append(curr);
    while (!(curr == from)) {
        curr = parent.get(curr);
        reverse_path.append(curr);
    }

    for (int i = reverse_path.get_length() - 1; i >= 0; --i) {
        path.append(reverse_path[i]);
    }
    return path;
}

#endif // GRAPH_TPP
