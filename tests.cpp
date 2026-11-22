#include <gtest/gtest.h>
#include <string>

#include "include/Graph/DirectedGraph.hpp"
#include "include/Graph/UndirectedGraph.hpp"

// Вспомогательная функция: проверка корректности топологического порядка
template <typename V, typename W>
bool is_valid_topological_order(const DirectedGraph<V, W>& graph, const mutable_array_sequence<V>& order) {
    if (order.get_length() != static_cast<int>(graph.get_vertex_count())) {
        return false;
    }

    for (int i = 0; i < order.get_length(); ++i) {
        V u = order[i];
        auto neighbors_res = graph.get_neighbors(u);
        if (!neighbors_res) return false;
        const auto& neighbors = *neighbors_res;

        for (int j = 0; j < neighbors.get_length(); ++j) {
            V v = neighbors[j].vertex;
            int v_idx = -1;
            for (int k = 0; k < order.get_length(); ++k) {
                if (order[k] == v) {
                    v_idx = k;
                    break;
                }
            }
            if (v_idx <= i) {
                return false; // u обязана предшествовать v
            }
        }
    }
    return true;
}

// ============================================================================
// ТЕСТЫ ОРИЕНТИРОВАННОГО ГРАФА (DirectedGraph)
// ============================================================================

TEST(directed_graph_test, add_and_contains_vertex) {
    DirectedGraph<int, int> graph;
    EXPECT_TRUE(graph.is_empty());
    EXPECT_EQ(graph.get_vertex_count(), 0u);

    EXPECT_TRUE(graph.add_vertex(1));
    EXPECT_TRUE(graph.add_vertex(2));
    EXPECT_TRUE(graph.add_vertex(3));

    // Повторное добавление вершины должно возвращать false
    EXPECT_FALSE(graph.add_vertex(2));

    EXPECT_EQ(graph.get_vertex_count(), 3u);
    EXPECT_FALSE(graph.is_empty());
    EXPECT_TRUE(graph.contains_vertex(1));
    EXPECT_TRUE(graph.contains_vertex(2));
    EXPECT_TRUE(graph.contains_vertex(3));
    EXPECT_FALSE(graph.contains_vertex(4));
    EXPECT_TRUE(graph.is_directed());
}

TEST(directed_graph_test, remove_vertex) {
    DirectedGraph<std::string, int> graph;
    graph.add_vertex("A");
    graph.add_vertex("B");
    graph.add_vertex("C");

    graph.add_edge("A", "B", 10);
    graph.add_edge("B", "C", 20);
    graph.add_edge("C", "A", 30);
    EXPECT_EQ(graph.get_edge_count(), 3u);

    // Удаляем вершину B
    EXPECT_TRUE(graph.remove_vertex("B"));
    EXPECT_EQ(graph.get_vertex_count(), 2u);
    EXPECT_FALSE(graph.contains_vertex("B"));

    // Связанные с B ребра должны удалиться (A->B и B->C)
    EXPECT_FALSE(graph.contains_edge("A", "B"));
    EXPECT_FALSE(graph.contains_edge("B", "C"));
    EXPECT_TRUE(graph.contains_edge("C", "A"));
    EXPECT_EQ(graph.get_edge_count(), 1u);

    // Удаление несуществующей вершины
    EXPECT_FALSE(graph.remove_vertex("Z"));
}

TEST(directed_graph_test, add_and_contains_edge) {
    DirectedGraph<int, double> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    EXPECT_TRUE(graph.add_edge(1, 2, 3.14));
    EXPECT_TRUE(graph.add_edge(2, 3, 2.71));

    // Повторное ребро и ребро к несуществующей вершине
    EXPECT_FALSE(graph.add_edge(1, 2, 5.0));
    EXPECT_FALSE(graph.add_edge(1, 99, 1.0));

    EXPECT_EQ(graph.get_edge_count(), 2u);
    EXPECT_TRUE(graph.contains_edge(1, 2));
    EXPECT_FALSE(graph.contains_edge(2, 1)); // В орграфе ребро направленное

    auto w1 = graph.get_edge_weight(1, 2);
    ASSERT_TRUE(w1.has_value());
    EXPECT_DOUBLE_EQ(*w1, 3.14);

    auto w_none = graph.get_edge_weight(2, 1);
    EXPECT_FALSE(w_none.has_value());
}

TEST(directed_graph_test, remove_edge_and_set_weight) {
    DirectedGraph<int, int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2, 100);

    EXPECT_TRUE(graph.set_edge_weight(1, 2, 200));
    auto w = graph.get_edge_weight(1, 2);
    ASSERT_TRUE(w.has_value());
    EXPECT_EQ(*w, 200);

    EXPECT_TRUE(graph.remove_edge(1, 2));
    EXPECT_EQ(graph.get_edge_count(), 0u);
    EXPECT_FALSE(graph.contains_edge(1, 2));
    EXPECT_FALSE(graph.remove_edge(1, 2));
}

TEST(directed_graph_test, in_and_out_degrees) {
    DirectedGraph<int, int> graph;
    for (int i = 1; i <= 4; ++i) graph.add_vertex(i);

    // 1 -> 2, 1 -> 3, 2 -> 3, 4 -> 3
    graph.add_edge(1, 2, 1);
    graph.add_edge(1, 3, 1);
    graph.add_edge(2, 3, 1);
    graph.add_edge(4, 3, 1);

    auto out1 = graph.get_out_degree(1);
    auto in1 = graph.get_in_degree(1);
    ASSERT_TRUE(out1.has_value() && in1.has_value());
    EXPECT_EQ(*out1, 2);
    EXPECT_EQ(*in1, 0);

    auto in3 = graph.get_in_degree(3);
    auto out3 = graph.get_out_degree(3);
    ASSERT_TRUE(in3.has_value() && out3.has_value());
    EXPECT_EQ(*in3, 3);
    EXPECT_EQ(*out3, 0);

    EXPECT_FALSE(graph.get_in_degree(99).has_value());
}

TEST(directed_graph_test, dfs_and_bfs_traversal) {
    DirectedGraph<int, int> graph;
    for (int i = 1; i <= 4; ++i) graph.add_vertex(i);

    // Ромб: 1 -> 2, 1 -> 3, 2 -> 4, 3 -> 4
    graph.add_edge(1, 2, 1);
    graph.add_edge(1, 3, 1);
    graph.add_edge(2, 4, 1);
    graph.add_edge(3, 4, 1);

    auto dfs_res = graph.dfs(1);
    ASSERT_TRUE(dfs_res.has_value());
    EXPECT_EQ(dfs_res->get_length(), 4);
    EXPECT_EQ((*dfs_res)[0], 1); // Стартовая вершина первая

    auto bfs_res = graph.bfs(1);
    ASSERT_TRUE(bfs_res.has_value());
    EXPECT_EQ(bfs_res->get_length(), 4);
    EXPECT_EQ((*bfs_res)[0], 1);
    EXPECT_EQ((*bfs_res)[3], 4); // Самая удаленная вершина последняя

    // Обход из несуществующей вершины
    EXPECT_FALSE(graph.dfs(99).has_value());
    EXPECT_FALSE(graph.bfs(99).has_value());
}

TEST(directed_graph_test, has_path_and_find_path) {
    DirectedGraph<int, int> graph;
    for (int i = 1; i <= 5; ++i) graph.add_vertex(i);

    // 1 -> 2 -> 3 -> 4, вершина 5 изолирована
    graph.add_edge(1, 2, 1);
    graph.add_edge(2, 3, 1);
    graph.add_edge(3, 4, 1);

    EXPECT_TRUE(graph.has_path(1, 4));
    EXPECT_FALSE(graph.has_path(4, 1)); // Обратного пути нет
    EXPECT_FALSE(graph.has_path(1, 5));

    auto path_res = graph.find_path(1, 4);
    ASSERT_TRUE(path_res.has_value());
    EXPECT_EQ(path_res->get_length(), 4);
    EXPECT_EQ((*path_res)[0], 1);
    EXPECT_EQ((*path_res)[1], 2);
    EXPECT_EQ((*path_res)[2], 3);
    EXPECT_EQ((*path_res)[3], 4);

    // Путь к самой себе
    auto self_path = graph.find_path(2, 2);
    ASSERT_TRUE(self_path.has_value());
    EXPECT_EQ(self_path->get_length(), 1);
    EXPECT_EQ((*self_path)[0], 2);

    // Несуществующий путь
    EXPECT_FALSE(graph.find_path(1, 5).has_value());
}

TEST(directed_graph_test, cycle_detection) {
    DirectedGraph<int, int> dag;
    for (int i = 1; i <= 4; ++i) dag.add_vertex(i);
    dag.add_edge(1, 2, 1);
    dag.add_edge(2, 3, 1);
    dag.add_edge(3, 4, 1);
    dag.add_edge(1, 3, 1);
    EXPECT_FALSE(dag.has_cycle());

    // Замыкаем цикл: 4 -> 2
    dag.add_edge(4, 2, 1);
    EXPECT_TRUE(dag.has_cycle());

    // Проверка петли (self-loop)
    DirectedGraph<int, int> loop_graph;
    loop_graph.add_vertex(10);
    EXPECT_FALSE(loop_graph.has_cycle());
    loop_graph.add_edge(10, 10, 1);
    EXPECT_TRUE(loop_graph.has_cycle());
}

TEST(directed_graph_test, topological_sort) {
    DirectedGraph<std::string, int> dag;
    dag.add_vertex("A");
    dag.add_vertex("B");
    dag.add_vertex("C");
    dag.add_vertex("D");

    // A -> B -> D, A -> C -> D
    dag.add_edge("A", "B", 1);
    dag.add_edge("A", "C", 1);
    dag.add_edge("B", "D", 1);
    dag.add_edge("C", "D", 1);

    auto order_res = dag.topological_sort();
    ASSERT_TRUE(order_res.has_value());
    EXPECT_EQ(order_res->get_length(), 4);
    EXPECT_TRUE(is_valid_topological_order(dag, *order_res));
    EXPECT_EQ((*order_res)[0], "A");
    EXPECT_EQ((*order_res)[3], "D");

    // В графе с циклом топологическая сортировка невозможна
    dag.add_edge("D", "A", 1);
    auto cycle_order = dag.topological_sort();
    EXPECT_FALSE(cycle_order.has_value());

    // Пустой граф
    DirectedGraph<int, int> empty_graph;
    auto empty_order = empty_graph.topological_sort();
    ASSERT_TRUE(empty_order.has_value());
    EXPECT_EQ(empty_order->get_length(), 0);
}

TEST(directed_graph_test, get_subgraph) {
    DirectedGraph<int, int> graph;
    for (int i = 1; i <= 5; ++i) graph.add_vertex(i);
    graph.add_edge(1, 2, 10);
    graph.add_edge(2, 3, 20);
    graph.add_edge(3, 4, 30);
    graph.add_edge(4, 5, 40);
    graph.add_edge(1, 3, 50);

    mutable_array_sequence<int> sub_vertices;
    sub_vertices.append(1);
    sub_vertices.append(2);
    sub_vertices.append(3);

    DirectedGraph<int, int>* sub = graph.get_subgraph(sub_vertices);
    ASSERT_NE(sub, nullptr);
    EXPECT_EQ(sub->get_vertex_count(), 3u);
    EXPECT_EQ(sub->get_edge_count(), 3u); // 1->2, 2->3, 1->3
    EXPECT_TRUE(sub->contains_edge(1, 2));
    EXPECT_TRUE(sub->contains_edge(2, 3));
    EXPECT_TRUE(sub->contains_edge(1, 3));
    EXPECT_FALSE(sub->contains_edge(3, 4));

    delete sub;
}

// ============================================================================
// ТЕСТЫ НЕОРИЕНТИРОВАННОГО ГРАФА (UndirectedGraph)
// ============================================================================

TEST(undirected_graph_test, add_and_contains_edge) {
    UndirectedGraph<int, int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    EXPECT_TRUE(graph.add_edge(1, 2, 10));

    // В неориентированном графе ребро существует в обе стороны
    EXPECT_TRUE(graph.contains_edge(1, 2));
    EXPECT_TRUE(graph.contains_edge(2, 1));
    EXPECT_EQ(graph.get_edge_count(), 1u);

    auto deg1 = graph.get_degree(1);
    auto deg2 = graph.get_degree(2);
    ASSERT_TRUE(deg1.has_value() && deg2.has_value());
    EXPECT_EQ(*deg1, 1u);
    EXPECT_EQ(*deg2, 1u);

    // Удаление ребра убирает его в обе стороны
    EXPECT_TRUE(graph.remove_edge(2, 1));
    EXPECT_EQ(graph.get_edge_count(), 0u);
    EXPECT_FALSE(graph.contains_edge(1, 2));
    EXPECT_FALSE(graph.contains_edge(2, 1));
}

TEST(undirected_graph_test, cycle_detection) {
    UndirectedGraph<int, int> tree;
    for (int i = 1; i <= 4; ++i) tree.add_vertex(i);
    tree.add_edge(1, 2, 1);
    tree.add_edge(2, 3, 1);
    tree.add_edge(2, 4, 1);
    EXPECT_FALSE(tree.has_cycle());

    // Замыкаем треугольник 1-2-3-1
    tree.add_edge(3, 1, 1);
    EXPECT_TRUE(tree.has_cycle());

    // Петля в неориентированном графе
    UndirectedGraph<int, int> loop_graph;
    loop_graph.add_vertex(5);
    loop_graph.add_edge(5, 5, 1);
    EXPECT_TRUE(loop_graph.has_cycle());
}

TEST(undirected_graph_test, connected_and_tree) {
    UndirectedGraph<int, int> graph;

    // Пустой граф и граф из 1 вершины — связны и являются деревом
    EXPECT_TRUE(graph.is_connected());
    EXPECT_TRUE(graph.is_tree());

    graph.add_vertex(1);
    EXPECT_TRUE(graph.is_connected());
    EXPECT_TRUE(graph.is_tree());

    // Добавляем несвязную вершину
    graph.add_vertex(2);
    EXPECT_FALSE(graph.is_connected());
    EXPECT_FALSE(graph.is_tree());

    // Соединяем ребром -> связное дерево
    graph.add_edge(1, 2, 1);
    EXPECT_TRUE(graph.is_connected());
    EXPECT_TRUE(graph.is_tree());

    // Добавляем цикл 1-2-3-1 -> связен, но не дерево
    graph.add_vertex(3);
    graph.add_edge(2, 3, 1);
    graph.add_edge(3, 1, 1);
    EXPECT_TRUE(graph.is_connected());
    EXPECT_FALSE(graph.is_tree());
}

TEST(undirected_graph_test, get_subgraph) {
    UndirectedGraph<int, int> graph;
    for (int i = 1; i <= 4; ++i) graph.add_vertex(i);
    graph.add_edge(1, 2, 10);
    graph.add_edge(2, 3, 20);
    graph.add_edge(3, 4, 30);

    mutable_array_sequence<int> sub_vertices;
    sub_vertices.append(1);
    sub_vertices.append(2);
    sub_vertices.append(3);

    UndirectedGraph<int, int>* sub = graph.get_subgraph(sub_vertices);
    ASSERT_NE(sub, nullptr);
    EXPECT_EQ(sub->get_vertex_count(), 3u);
    EXPECT_EQ(sub->get_edge_count(), 2u); // 1-2 и 2-3
    EXPECT_TRUE(sub->contains_edge(1, 2));
    EXPECT_TRUE(sub->contains_edge(2, 3));
    EXPECT_FALSE(sub->contains_edge(3, 4));

    delete sub;
}

// ============================================================================
// ТЕСТЫ ЭТАПА 2: ДОМЕННЫЕ СУЩНОСТИ И КАТАЛОГ
// ============================================================================

#include "include/PackageManager/Domain/Version.hpp"
#include "include/PackageManager/Domain/Requirement.hpp"
#include "include/PackageManager/Domain/Package.hpp"
#include "include/PackageManager/Catalog/PackageCatalog.hpp"
#include "include/PackageManager/Storage/InstalledRegistry.hpp"

using namespace pm;

TEST(domain_version_test, parse_and_compare) {
    auto v1 = Version::parse("1.2.3");
    ASSERT_TRUE(v1.has_value());
    EXPECT_EQ(v1->major, 1);
    EXPECT_EQ(v1->minor, 2);
    EXPECT_EQ(v1->patch, 3);
    EXPECT_EQ(v1->to_string(), "1.2.3");

    // Сравнение версий (SemVer)
    auto v2 = Version::parse("1.10.0");
    auto v3 = Version::parse("1.9.5");
    ASSERT_TRUE(v2.has_value() && v3.has_value());
    EXPECT_TRUE(*v2 > *v3); // 1.10 > 1.9
    EXPECT_TRUE(*v1 < *v2);
    EXPECT_TRUE(*v1 == Version(1, 2, 3));

    // Ошибки парсинга
    EXPECT_FALSE(Version::parse("").has_value());
    EXPECT_FALSE(Version::parse("1.a.3").has_value());
    EXPECT_FALSE(Version::parse("1.2.3.4").has_value());
    EXPECT_FALSE(Version::parse("1..2").has_value());
}

TEST(domain_requirement_test, parse_and_satisfies) {
    auto req1 = Requirement::parse("libfoo >= 1.2.0");
    ASSERT_TRUE(req1.has_value());
    EXPECT_EQ(req1->package_name, "libfoo");
    EXPECT_EQ(req1->op, VersionOp::GREATER_EQUAL);
    EXPECT_EQ(req1->version, Version(1, 2, 0));

    EXPECT_TRUE(req1->satisfies(Version(1, 2, 0)));
    EXPECT_TRUE(req1->satisfies(Version(2, 0, 0)));
    EXPECT_FALSE(req1->satisfies(Version(1, 1, 9)));

    // Требование без оператора (любая версия)
    auto req_any = Requirement::parse("graphics-engine");
    ASSERT_TRUE(req_any.has_value());
    EXPECT_EQ(req_any->op, VersionOp::ANY);
    EXPECT_TRUE(req_any->satisfies(Version(0, 0, 1)));
    EXPECT_TRUE(req_any->satisfies(Version(99, 99, 99)));

    // Ошибочные требования
    EXPECT_FALSE(Requirement::parse("").has_value());
    EXPECT_FALSE(Requirement::parse(">= 1.0.0").has_value());
    EXPECT_FALSE(Requirement::parse("libfoo >=").has_value());
}

TEST(domain_package_test, validation_and_builder) {
    Package p("valid-name_1", Version(1, 0, 0), "Описание");
    p.add_dependency(Requirement("libfoo", VersionOp::GREATER_EQUAL, Version(1, 0, 0)))
     .add_replaces("old-package")
     .add_file("bin/app", "echo hello", true)
     .add_pre_install("echo Pre")
     .add_post_install("echo Post");

    auto dep_res = p.add_dependency("libbar >= 2.0.0");
    EXPECT_TRUE(dep_res.has_value());

    auto conf_res = p.add_conflict("legacy-lib");
    EXPECT_TRUE(conf_res.has_value());

    auto bad_dep = p.add_dependency("invalid requirement >=");
    EXPECT_FALSE(bad_dep.has_value());

    auto bad_conf = p.add_conflict("invalid conflict >=");
    EXPECT_FALSE(bad_conf.has_value());

    EXPECT_EQ(p.get_name(), "valid-name_1");
    EXPECT_EQ(p.get_version(), Version(1, 0, 0));
    EXPECT_EQ(p.get_description(), "Описание");
    EXPECT_EQ(p.get_dependencies().get_length(), 2);
    EXPECT_EQ(p.get_conflicts().get_length(), 1);
    EXPECT_EQ(p.get_replaces().get_length(), 1);
    EXPECT_EQ(p.get_files().get_length(), 1);
    EXPECT_TRUE(p.validate().has_value());

    // Некорректное имя пакета
    Package bad_name("-bad-start", Version(1, 0, 0));
    EXPECT_FALSE(bad_name.validate().has_value());

    // Безопасность путей (Sandbox)
    Package bad_path("test-pkg", Version(1, 0, 0));
    bad_path.add_file("/etc/shadow", "secret");
    EXPECT_FALSE(bad_path.validate().has_value());

    Package traversal_pkg("test-pkg", Version(1, 0, 0));
    traversal_pkg.add_file("../outside.txt", "data");
    EXPECT_FALSE(traversal_pkg.validate().has_value());

    // Дубликат файла внутри одного пакета
    Package dup_file("test-pkg", Version(1, 0, 0));
    dup_file.add_file("file.txt", "a").add_file("file.txt", "b");
    EXPECT_FALSE(dup_file.validate().has_value());
}

TEST(catalog_test, add_find_and_filter) {
    PackageCatalog catalog;
    EXPECT_EQ(catalog.get_package_count(), 0u);

    Package p1("libmath", Version(1, 0, 0));
    Package p2("libmath", Version(1, 5, 0));
    Package p3("libmath", Version(2, 0, 0));
    Package p_other("editor", Version(1, 0, 0));

    EXPECT_TRUE(catalog.add_package(p1).has_value());
    EXPECT_TRUE(catalog.add_package(p2).has_value());
    EXPECT_TRUE(catalog.add_package(p3).has_value());
    EXPECT_TRUE(catalog.add_package(p_other).has_value());

    // Отклонение дубликата той же версии
    EXPECT_FALSE(catalog.add_package(p1).has_value());

    EXPECT_EQ(catalog.get_package_count(), 2u); // 2 уникальных имени: libmath, editor
    EXPECT_EQ(catalog.get_total_version_count(), 4u); // 4 версии всего
    EXPECT_TRUE(catalog.contains("libmath"));
    EXPECT_TRUE(catalog.contains("libmath", Version(1, 5, 0)));
    EXPECT_FALSE(catalog.contains("libmath", Version(3, 0, 0)));

    // Поиск наилучшей подходящей версии
    auto match_gte = catalog.find_best_match(Requirement("libmath", VersionOp::GREATER_EQUAL, Version(1, 0, 0)));
    ASSERT_TRUE(match_gte.has_value());
    EXPECT_EQ(match_gte->get_version(), Version(2, 0, 0)); // Максимальная подходящая

    auto match_lt = catalog.find_best_match(Requirement("libmath", VersionOp::LESS, Version(2, 0, 0)));
    ASSERT_TRUE(match_lt.has_value());
    EXPECT_EQ(match_lt->get_version(), Version(1, 5, 0));

    auto match_none = catalog.find_best_match(Requirement("libmath", VersionOp::GREATER, Version(5, 0, 0)));
    EXPECT_FALSE(match_none.has_value());
}

TEST(installed_registry_test, install_conflict_replace_and_serialize) {
    InstalledRegistry registry;
    EXPECT_EQ(registry.get_count(), 0u);

    Package lib("libgraphics", Version(1, 0, 0));
    lib.add_file("lib/libgraphics.txt", "content");
    EXPECT_TRUE(registry.record_install(lib).has_value());
    EXPECT_TRUE(registry.is_installed("libgraphics"));
    EXPECT_TRUE(registry.has_file_owner("lib/libgraphics.txt"));

    auto owner = registry.get_file_owner("lib/libgraphics.txt");
    ASSERT_TRUE(owner.has_value());
    EXPECT_EQ(*owner, "libgraphics");

    // Конфликт файлов: другой пакет пытается занять тот же путь без replaces
    Package intruder("rogue-pkg", Version(1, 0, 0));
    intruder.add_file("lib/libgraphics.txt", "hijack");
    EXPECT_FALSE(registry.record_install(intruder).has_value());

    // Пакет с заменой (replaces) может переопределить файлы старого пакета
    Package upgraded_lib("super-graphics", Version(2, 0, 0));
    upgraded_lib.add_replaces("libgraphics")
                .add_file("lib/libgraphics.txt", "v2-content");
    EXPECT_TRUE(registry.record_install(upgraded_lib).has_value());
    EXPECT_FALSE(registry.is_installed("libgraphics")); // Старый пакет вытеснен
    EXPECT_TRUE(registry.is_installed("super-graphics"));
    EXPECT_EQ(*registry.get_file_owner("lib/libgraphics.txt"), "super-graphics");

    // Сериализация и десериализация
    std::string state_data = registry.serialize();
    EXPECT_FALSE(state_data.empty());

    auto restored = InstalledRegistry::deserialize(state_data);
    ASSERT_TRUE(restored.has_value());
    EXPECT_EQ(restored->get_count(), 1u);
    EXPECT_TRUE(restored->is_installed("super-graphics"));
    EXPECT_EQ(*restored->get_file_owner("lib/libgraphics.txt"), "super-graphics");

    // Удаление пакета
    EXPECT_TRUE(registry.record_remove("super-graphics").has_value());
    EXPECT_EQ(registry.get_count(), 0u);
    EXPECT_FALSE(registry.has_file_owner("lib/libgraphics.txt"));
}

