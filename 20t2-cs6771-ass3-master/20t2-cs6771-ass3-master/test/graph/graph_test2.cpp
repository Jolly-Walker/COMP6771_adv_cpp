#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

TEST_CASE("Basic tests and Accessors") {
	SECTION("insert_node and is_node") {
		auto g = gdwg::graph<int, std::string>{};
		auto n = 5;
		g.insert_node(n);
		CHECK(g.is_node(n));
		CHECK_FALSE(g.is_node(1));
	}

	SECTION("initiliser list constructor test") {
		auto const g = gdwg::graph<int, int>{1, 2, 3};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}

	SECTION("insert_edge and is_connected") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		CHECK(g.insert_edge(1, 2, 5));
		CHECK_FALSE(g.insert_edge(1, 2, 5));
		CHECK(g.is_connected(1, 2));
		CHECK_FALSE(g.is_connected(1, 3));
	}

	SECTION("weights") {
		auto n1 = std::string("n1");
		auto n2 = std::string("n2");
		auto n3 = std::string("n3");
		auto g = gdwg::graph<std::string, std::string>{n1, n2, n3};
		CHECK(g.is_node(n1));
		CHECK(g.is_node(n2));
		CHECK(g.insert_edge(n1, n2, n1));
		CHECK_FALSE(g.insert_edge(n1, n2, n1));
		CHECK(g.is_connected(n1, n2));
		CHECK(g.weights(n1, n2) == std::vector<std::string>{n1});
	}

	SECTION("clear") {
		auto g = gdwg::graph<int, std::string>{1, 2};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		g.clear();
		CHECK_FALSE(g.is_node(1));
		CHECK_FALSE(g.is_node(2));
	}

	SECTION("empty") {
		auto g = gdwg::graph<int, std::string>{};
		CHECK(g.empty());
		g.insert_node(5);
		CHECK_FALSE(g.empty());
		g.clear();
		CHECK(g.empty());
	}

	SECTION("nodes") {
		auto g = gdwg::graph<int, int>{2, 3, 4};
		CHECK(g.nodes() == std::vector<int>{2, 3, 4});
		g.insert_node(1);
		CHECK(g.nodes() == std::vector<int>{1, 2, 3, 4});
		g.replace_node(2, 7);
		CHECK(g.nodes() == std::vector<int>{1, 3, 4, 7});
	}

	SECTION("connections") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		CHECK(std::empty(g.connections(1)));
		g.insert_edge(1, 3, 9);
		g.insert_edge(1, 2, 9);
		CHECK(g.connections(1) == std::vector<int>{2, 3});
	}

	SECTION("==operator") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		auto g2 = gdwg::graph<int, int>{1, 2, 3};
		g2.insert_edge(1, 2, 5);
		g2.insert_edge(1, 2, 6);
		CHECK(g == g2);
		g.erase_edge(1, 2, 6);
		CHECK_FALSE(g == g2);
	}
}

TEST_CASE("Constructors") {
	SECTION("forward iterator constructor test") {
		auto const value_type_vector = std::vector<int>{1, 2, 3};
		auto const g = gdwg::graph<int, int>{value_type_vector.begin(), value_type_vector.end()};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}

	SECTION("value_type constructor test") {
		auto const value_type_vector =
		   std::vector<gdwg::graph<int, int>::value_type>{{1, 2, 1}, {2, 3, 4}};
		auto const g = gdwg::graph<int, int>{value_type_vector.begin(), value_type_vector.end()};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		CHECK(g.weights(1, 2) == std::vector<int>{1});
		CHECK(g.weights(2, 3) == std::vector<int>{4});
	}

	SECTION("move constructor test") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		auto g_moved = gdwg::graph<int, int>(std::move(g));

		CHECK(g_moved.is_node(1));
		CHECK(g_moved.is_node(2));
		CHECK(g_moved.is_node(3));

		// NOLINTNEXTLINE(bugprone-use-after-move, clang-analyzer-cplusplus.Move)
		CHECK(g.empty());
	}

	SECTION("Copy constructor test") {
		auto const value_type_vector = std::vector<gdwg::graph<int, int>::value_type>{{1, 2, 5}};
		auto const g =
		   gdwg::graph<int, int>(std::cbegin(value_type_vector), std::cend(value_type_vector));
		auto g2 = gdwg::graph<int, int>(g);
		CHECK(g == g2);
		g2.insert_node(5);
		CHECK(g2.is_node(1));
		CHECK(g2.is_node(2));
		CHECK(g2.weights(1, 2) == std::vector<int>{5});
		CHECK(g2.is_node(5));
		CHECK_FALSE(g.is_node(5));
	}
}

TEST_CASE("Assignment") {
	SECTION("Copy assignment") {
		auto const v = std::vector<gdwg::graph<int, int>::value_type>{
		   {4, 1, -4},
		   {4, 1, 2},
		};
		auto const g = gdwg::graph<int, int>(v.begin(), v.end());
		auto g2 = g;
		CHECK(g == g2);
		CHECK(g2.weights(4, 1) == std::vector<int>{-4, 2});
		g2.insert_node(5);
		CHECK(g2.is_node(5));
		CHECK_FALSE(g.is_node(5));
	}

	SECTION("Move assignment") {
		auto const v = std::vector<gdwg::graph<int, int>::value_type>{
		   {4, 1, -4},
		   {4, 1, 2},
		};
		auto g = gdwg::graph<int, int>(v.begin(), v.end());
		auto g2 = std::move(g);
		CHECK(g2.weights(4, 1) == std::vector<int>{-4, 2});

		// NOLINTNEXTLINE(bugprone-use-after-move, clang-analyzer-cplusplus.Move)
		CHECK(g.empty());
	}
}
TEST_CASE("Modifiers") {
	SECTION("replace_node") {
		auto g = gdwg::graph<int, int>{1, 2, 3};

		g.insert_edge(1, 2, 5);
		g.insert_edge(3, 1, 6);
		g.insert_edge(1, 1, 2);
		g.replace_node(1, 11);
		CHECK(g.is_node(11));
		CHECK_FALSE(g.is_node(1));
		CHECK(g.connections(11) == std::vector<int>{2, 11});
		CHECK(g.connections(3) == std::vector<int>{11});
	}

	SECTION("merge_replace_node") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 3, 5);
		g.insert_edge(2, 3, 6);
		g.insert_edge(3, 1, 1);
		g.insert_edge(3, 2, 1);
		g.insert_edge(3, 1, 2);
		g.merge_replace_node(1, 2);
		CHECK(g.weights(2, 3) == std::vector<int>{5, 6});
		CHECK(g.weights(3, 2) == std::vector<int>{1, 2});
	}

	SECTION("erase_node") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		CHECK(g.is_node(1));
		CHECK(g.erase_node(1));
		CHECK_FALSE(g.erase_node(1));
		CHECK_FALSE(g.is_node(1));
	}

	SECTION("erase_edge") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		CHECK(g.weights(1, 2) == std::vector<int>{5, 6});
		CHECK(g.erase_edge(1, 2, 5));
		CHECK_FALSE(g.erase_edge(1, 2, 4));
		CHECK(g.weights(1, 2) == std::vector<int>{6});
	}
}

TEST_CASE("Extractor") {
	SECTION("<<operator empty") {
		auto const g = gdwg::graph<int, int>();
		auto const actual_string = fmt::format("{}", g);
		CHECK(std::empty(g));
	}
	SECTION("<<operator") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{
		   {4, 1, -4},
		   {3, 2, 2},
		   {2, 4, 2},
		   {2, 1, 1},
		   {6, 2, 5},
		   {6, 3, 10},
		   {1, 5, -1},
		   {3, 6, -8},
		   {4, 5, 3},
		   {5, 2, 7},
		};

		auto g = graph(v.begin(), v.end());
		g.insert_node(64);
		auto const actual_string = fmt::format("{}", g);
		auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");
		CHECK(actual_string == expected_output);
	}
}

TEST_CASE("Iterators") {
	SECTION("begin") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		auto g_begin_it = g.begin();
		CHECK((*g_begin_it) == std::tuple<int, int, int>{1, 2, 5});
	}

	SECTION("iterator ==operator and end") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		auto g_begin_it = g.begin();
		CHECK(g_begin_it == g.begin());
		CHECK_FALSE(g_begin_it == g.end());

		auto g2 = gdwg::graph<int, int>{1, 2, 3};
		g2.insert_edge(1, 2, 5);
		CHECK_FALSE(g_begin_it == g2.begin());
	}

	SECTION("iterator increment order") {
		auto const v = std::vector<gdwg::graph<int, int>::value_type>{{1, 12, 3},
		                                                              {1, 7, 4},
		                                                              {7, 21, 13},
		                                                              {12, 19, 16},
		                                                              {14, 14, 1},
		                                                              {14, 14, 0}};
		auto const g = gdwg::graph<int, int>(v.begin(), v.end());
		auto it = g.begin();

		CHECK(*it == std::tuple{1, 7, 4});
		++it;
		CHECK(*it == std::tuple{1, 12, 3});
		++it;
		CHECK(*it == std::tuple{7, 21, 13});
		++it;
		CHECK(*it == std::tuple{12, 19, 16});
		++it;
		CHECK(*it == std::tuple{14, 14, 0});
		++it;
		CHECK(*it == std::tuple{14, 14, 1});
	}

	SECTION("iterator *operator") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		auto const g_begin_it = g.begin();
		CHECK((*g_begin_it) == std::tuple{1, 2, 5});
	}

	SECTION("iterator decrement") {
		auto const v = std::vector<gdwg::graph<int, int>::value_type>{{1, 12, 3},
		                                                              {1, 7, 4},
		                                                              {7, 21, 13},
		                                                              {12, 19, 16},
		                                                              {14, 14, 1},
		                                                              {14, 14, 0}};
		auto const g = gdwg::graph<int, int>(v.begin(), v.end());
		auto it = g.end();

		--it;
		CHECK(*it == std::tuple{14, 14, 1});
		--it;
		CHECK(*it == std::tuple{14, 14, 0});
		--it;
		CHECK(*it == std::tuple{12, 19, 16});
		--it;
		CHECK(*it == std::tuple{7, 21, 13});
		--it;
		CHECK(*it == std::tuple{1, 12, 3});
		--it;
		CHECK(*it == std::tuple{1, 7, 4});
	}

	SECTION("post increment and decrement") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 5);
		auto g_begin_it = g.begin();
		auto begin_copy = g_begin_it++;
		CHECK(begin_copy == g.begin());
		auto second_copy = g_begin_it--;
		CHECK(*second_copy == std::tuple(2, 2, 5));
	}
}

TEST_CASE("find and erase with iterator") {
	SECTION("find") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 6);
		g.insert_edge(2, 2, 5);
		auto it = g.find(1, 2, 5);
		CHECK((*it) == std::tuple{1, 2, 5});
		++it;
		CHECK((*it) == std::tuple{2, 2, 5});
	}
	SECTION("erase with iterator") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 5);
		auto it = g.find(1, 2, 5);
		auto next_it = g.erase_edge(it);
		CHECK_FALSE(g.is_connected(1, 2));
		CHECK(*next_it == std::tuple(2, 2, 5));
	}

	SECTION("erase with 2 iterators") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 5);
		auto const it = g.erase_edge(g.begin(), g.end());
		CHECK(g.begin() == g.end());
		CHECK(it == g.end());
	}
}

TEST_CASE("Exceptions") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	SECTION("Insert_edge") {
		CHECK_THROWS_MATCHES(g.insert_edge(1, 4, 5),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::insert_edge "
		                                              "when either src or dst node does not exist"));
		CHECK_THROWS_MATCHES(g.insert_edge(33, 1, 5),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::insert_edge "
		                                              "when either src or dst node does not exist"));
	}

	SECTION("replace_node") {
		CHECK_THROWS_MATCHES(g.replace_node(7, 4),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::replace_node "
		                                              "on a node that doesn't exist"));
	}

	SECTION("merge_replace_node") {
		CHECK_THROWS_MATCHES(g.merge_replace_node(7, 1),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, "
		                                              "E>::merge_replace_node on old or new data if "
		                                              "they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.merge_replace_node(1, 9),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, "
		                                              "E>::merge_replace_node on old or new data if "
		                                              "they don't exist in the graph"));
	}

	SECTION("erase_edge") {
		CHECK_THROWS_MATCHES(g.erase_edge(1, 44, 2),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::erase_edge on "
		                                              "src or dst if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.erase_edge(55, 3, 2),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::erase_edge on "
		                                              "src or dst if they don't exist in the graph"));
	}

	SECTION("is_connected") {
		CHECK_THROWS_MATCHES(g.is_connected(1, 5),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, "
		                                              "E>::is_connected if src or "
		                                              "dst node don't exist in the "
		                                              "graph"));
		CHECK_THROWS_MATCHES(g.is_connected(5, 1),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, "
		                                              "E>::is_connected if src or "
		                                              "dst node don't exist in the "
		                                              "graph"));
	}

	SECTION("weights") {
		CHECK_THROWS_MATCHES(g.weights(1, 22),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::weights if src "
		                                              "or dst node don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.weights(33, 2),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::weights if src "
		                                              "or dst node don't exist in the graph"));
	}

	SECTION("connections") {
		CHECK_THROWS_MATCHES(g.connections(77),
		                     std::runtime_error,
		                     Catch::Matchers::Message("Cannot call gdwg::graph<N, E>::connections if "
		                                              "src doesn't exist in the graph"));
	}
}
