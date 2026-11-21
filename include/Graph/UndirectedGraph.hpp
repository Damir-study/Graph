#ifndef UNDIRECTED_GRAPH_HPP
#define UNDIRECTED_GRAPH_HPP

#include <cstddef>
#include <expected>
#include <string>

#include "Graph.hpp"
#include "../../TreeStructures/include/Dictionary/BinaryTreeDictionaryStorage.hpp"
#include "../../TreeStructures/include/BinaryTree/BalancedTree.hpp"

template <typename V, typename W>
class UndirectedGraph : public Graph<V, W> {
public:
    using typename Graph<V, W>::Neighbor;
    using typename Graph<V, W>::NeighborList;
    using typename Graph<V, W>::Storage;

    UndirectedGraph();
    UndirectedGraph(Storage* storage);

    UndirectedGraph(const UndirectedGraph& other) = default;
    UndirectedGraph& operator=(const UndirectedGraph& other) = default;
    UndirectedGraph(UndirectedGraph&& other) = default;
    UndirectedGraph& operator=(UndirectedGraph&& other) = default;

    ~UndirectedGraph() override = default;

    // Работа с рёбрами
    bool add_edge(const V& from, const V& to, const W& weight) override;
    bool remove_edge(const V& from, const V& to) override;
    bool set_edge_weight(const V& from, const V& to, const W& weight) override;

    bool is_directed() const override;

    // Степень вершины
    std::expected<std::size_t, std::string> get_degree(const V& vertex) const;

    // Анализ графа
    bool has_cycle() const override;
    bool is_connected() const;
    bool is_tree() const;

    // Построение порождённого подграфа
    UndirectedGraph<V, W>* get_subgraph(const mutable_array_sequence<V>& vertices) const override;
};

#include "../../src/Graph/UndirectedGraph.tpp"

#endif // UNDIRECTED_GRAPH_HPP
