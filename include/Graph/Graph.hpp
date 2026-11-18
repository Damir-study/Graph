#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cstddef>

#include "../../TreeStructures/include/Dictionary/Dictionary.hpp"
#include "../../Sequence/mutable_array_sequence.h"

template <typename V, typename W>
class Graph {
protected:
    struct Neighbor {
        V vertex;
        W weight;
    };

    using NeighborList = mutable_array_sequence<Neighbor>;
    using Storage = IDictionaryStorage<V, NeighborList>;

    Dictionary<V, NeighborList> adjacency;
    mutable_array_sequence<V> vertex_list;
    std::size_t edge_count = 0;

    Graph(Storage* storage);

public:
    virtual ~Graph() = default;

    // Вершины
    bool add_vertex(const V& vertex);
    bool remove_vertex(const V& vertex);
    bool contains_vertex(const V& vertex) const;

    // Рёбра
    virtual bool add_edge(
        const V& from,
        const V& to,
        const W& weight
    ) = 0;

    virtual bool remove_edge(
        const V& from,
        const V& to
    ) = 0;

    bool contains_edge(const V& from, const V& to) const;

    const W& get_edge_weight(
        const V& from,
        const V& to
    ) const;

    // Доступ к данным
    const mutable_array_sequence<V>& get_vertices() const;

    // Информация о графе
    std::size_t get_vertex_count() const;
    std::size_t get_edge_count() const;
    bool is_empty() const;

    virtual bool is_directed() const = 0;

    // Очистка
    void clear();
};

#include "../../src/Graph/Graph.tpp"

#endif // GRAPH_HPP
