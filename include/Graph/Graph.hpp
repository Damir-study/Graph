#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cstddef>
#include <expected>
#include <string>

#include "../../TreeStructures/include/Dictionary/Dictionary.hpp"
#include "../../Sequence/mutable_array_sequence.h"

// V — тип вершины; W — тип веса ребра.
template <typename V, typename W>
class Graph {
public:
    struct Neighbor {
        V vertex;
        W weight;
    };

    using NeighborList = mutable_array_sequence<Neighbor>;
    using Storage = IDictionaryStorage<V, NeighborList>;
protected:
    Dictionary<V, NeighborList> adjacency;
    mutable_array_sequence<V> vertex_list;
    std::size_t edge_count = 0;

    Graph(Storage* storage);

    Graph(const Graph& other);
    Graph& operator=(const Graph& other);
    Graph(Graph&& other);
    Graph& operator=(Graph&& other);

public:
    virtual ~Graph() = default;

    // вершины
    bool add_vertex(const V& vertex);
    bool remove_vertex(const V& vertex);
    bool contains_vertex(const V& vertex) const;

    // ребра
    virtual bool add_edge(const V& from, const V& to, const W& weight) = 0;
    virtual bool remove_edge(const V& from, const V& to) = 0;
    virtual bool set_edge_weight(const V& from, const V& to, const W& weight) = 0;

    bool contains_edge(const V& from, const V& to) const;
    std::expected<W, std::string> get_edge_weight(const V& from, const V& to) const;

    const mutable_array_sequence<V>& get_vertices() const;
    std::expected<NeighborList, std::string> get_neighbors(const V& vertex) const;

    std::size_t get_vertex_count() const;
    std::size_t get_edge_count() const;
    bool is_empty() const;
    virtual bool is_directed() const = 0;

    void clear_edges();
    void clear();

    // Обходы. Результат — вершины в порядке посещения.
    std::expected<mutable_array_sequence<V>, std::string> dfs(const V& start) const;
    std::expected<mutable_array_sequence<V>, std::string> bfs(const V& start) const;

    // Достижимость и поиск пути.
    bool has_path(const V& from, const V& to) const;
    std::expected<mutable_array_sequence<V>, std::string> find_path(const V& from, const V& to) const;

    // Анализ структуры графа.
    virtual bool has_cycle() const = 0;

    // Порождённый подграф: выбранные вершины и все рёбра между ними.
    virtual Graph<V, W>* get_subgraph(const mutable_array_sequence<V>& vertices) const = 0;
};

#include "../../src/Graph/Graph.tpp"

#endif // GRAPH_HPP
