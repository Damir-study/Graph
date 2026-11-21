#ifndef DIRECTED_GRAPH_HPP
#define DIRECTED_GRAPH_HPP

#include "Graph.hpp"
#include "../../TreeStructures/include/Dictionary/BinaryTreeDictionaryStorage.hpp"
#include "../../TreeStructures/include/BinaryTree/BalancedTree.hpp"

template <typename V, typename W>
class DirectedGraph : public Graph<V, W> {
public:
    using typename Graph<V, W>::Neighbor;
    using typename Graph<V, W>::NeighborList;
    using typename Graph<V, W>::Storage;

    DirectedGraph();
    DirectedGraph(Storage* storage);

    DirectedGraph(const DirectedGraph& other) = default;
    DirectedGraph& operator=(const DirectedGraph& other) = default;
    DirectedGraph(DirectedGraph&& other) = default;
    DirectedGraph& operator=(DirectedGraph&& other) = default;

    ~DirectedGraph() override = default;

    // Работа с дугами
    bool add_edge(const V& from, const V& to, const W& weight) override;
    bool remove_edge(const V& from, const V& to) override;
    bool set_edge_weight(const V& from, const V& to, const W& weight) override;

    bool is_directed() const override;

    // Степени и соседи
    std::size_t get_in_degree(const V& vertex) const;
    std::size_t get_out_degree(const V& vertex) const;

    // Анализ графа
    bool has_cycle() const override;

    // Построение порождённого подграфа
    DirectedGraph<V, W>* get_subgraph(const mutable_array_sequence<V>& vertices) const override;
};

#include "../../src/Graph/DirectedGraph.tpp"

#endif // DIRECTED_GRAPH_HPP
