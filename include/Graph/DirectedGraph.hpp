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
    explicit DirectedGraph(Storage* storage);
    ~DirectedGraph() override = default;

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

#include "../../src/Graph/DirectedGraph.tpp"

#endif // DIRECTED_GRAPH_HPP
