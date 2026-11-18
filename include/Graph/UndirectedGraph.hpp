#ifndef UNDIRECTED_GRAPH_HPP
#define UNDIRECTED_GRAPH_HPP

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
    explicit UndirectedGraph(Storage* storage);
    ~UndirectedGraph() override = default;

    bool add_edge(
        const V& from,
        const V& to,
        const W& weight
    ) override;

    bool remove_edge(
        const V& from,
        const V& to
    ) override;

    bool is_directed() const override;
};

#include "../../src/Graph/UndirectedGraph.tpp"

#endif // UNDIRECTED_GRAPH_HPP
