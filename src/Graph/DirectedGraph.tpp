#ifndef DIRECTED_GRAPH_TPP
#define DIRECTED_GRAPH_TPP

#include "../../include/Graph/DirectedGraph.hpp"

#include <expected>
#include <string>

template <typename V, typename W>
DirectedGraph<V, W>::DirectedGraph()
    : Graph<V, W>(new BinaryTreeDictionaryStorage<V, NeighborList>(
          new BalancedTree<DictionaryPair<V, NeighborList>>())) {}

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
bool DirectedGraph<V, W>::set_edge_weight(const V& from, const V& to, const W& weight) {
    if (!this->contains_vertex(from) || !this->contains_vertex(to)) return false;

    NeighborList& neighbors = this->adjacency.get(from);
    for (int i = 0; i < neighbors.get_length(); ++i) {
        if (neighbors[i].vertex == to) {
            neighbors[i].weight = weight;
            return true;
        }
    }
    return false;
}

template <typename V, typename W>
bool DirectedGraph<V, W>::is_directed() const {
    return true;
}

template <typename V, typename W>
std::expected<std::size_t, std::string> DirectedGraph<V, W>::get_in_degree(const V& vertex) const {
    if (!this->contains_vertex(vertex)) {
        return std::unexpected("Вершина не найдена в графе");
    }
    std::size_t count = 0;
    const mutable_array_sequence<V>& vertices = this->get_vertices();
    for (int i = 0; i < vertices.get_length(); ++i) {
        const NeighborList& neighbors = this->adjacency.get(vertices[i]);
        for (int j = 0; j < neighbors.get_length(); ++j) {
            if (neighbors[j].vertex == vertex) {
                count++;
            }
        }
    }
    return count;
}

template <typename V, typename W>
std::expected<std::size_t, std::string> DirectedGraph<V, W>::get_out_degree(const V& vertex) const {
    if (!this->contains_vertex(vertex)) {
        return std::unexpected("Вершина не найдена в графе");
    }
    return static_cast<std::size_t>(this->adjacency.get(vertex).get_length());
}

template <typename V, typename W>
bool DirectedGraph<V, W>::has_cycle() const {
    struct Frame {
        V vertex;
        int next_neighbor;
    };

    // Цвета: 0 = белый (не посещён), 1 = серый (в текущем пути стека), 2 = чёрный (полностью обработан)
    Dictionary<V, int> color(
        new BinaryTreeDictionaryStorage<V, int>(
            new BalancedTree<DictionaryPair<V, int>>()));

    const mutable_array_sequence<V>& vertices = this->get_vertices();
    for (int i = 0; i < vertices.get_length(); ++i) {
        color.add(vertices[i], 0);
    }

    mutable_array_sequence<Frame> stack;

    for (int i = 0; i < vertices.get_length(); ++i) {
        const V& start_vertex = vertices[i];
        if (color.get(start_vertex) != 0) {
            continue;
        }

        color.get(start_vertex) = 1;
        stack.append(Frame{start_vertex, 0});

        while (stack.get_length() > 0) {
            int top_idx = stack.get_length() - 1;
            V u = stack[top_idx].vertex;
            int next_idx = stack[top_idx].next_neighbor;
            const NeighborList& neighbors = this->adjacency.get(u);

            if (next_idx < neighbors.get_length()) {
                stack[top_idx].next_neighbor++;
                const V next_vertex = neighbors[next_idx].vertex;
                int next_color = color.get(next_vertex);

                if (next_color == 1) {
                    return true; // Обратное ребро к вершине на стеке -> обнаружен цикл
                }
                if (next_color == 0) {
                    color.get(next_vertex) = 1;
                    stack.append(Frame{next_vertex, 0});
                }
            } else {
                color.get(u) = 2; // Все исходящие рёбра исследованы
                stack.pop();
            }
        }
    }

    return false;
}

template <typename V, typename W>
DirectedGraph<V, W>* DirectedGraph<V, W>::get_subgraph(const mutable_array_sequence<V>& vertices) const {
    DirectedGraph<V, W>* sub = new DirectedGraph<V, W>();

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
            if (in_sub.contains_key(v)) {
                sub->add_edge(u, v, neighbors[j].weight);
            }
        }
    }

    return sub;
}

#endif // DIRECTED_GRAPH_TPP
