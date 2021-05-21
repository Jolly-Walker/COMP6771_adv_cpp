#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <__functional_base>
#include <algorithm>
#include <concepts/concepts.hpp>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <range/v3/iterator.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility.hpp>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

namespace gdwg {

	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //
	   class graph {
	public:
		class iterator;
		struct value_type {
			N from;
			N to;
			E weight;
		};

		struct edge {
			std::shared_ptr<N> from;
			std::shared_ptr<N> to;
			E weight;
		};

		graph() noexcept = default;

		graph(std::initializer_list<N> il)
		: graph{graph(il.begin(), il.end())} {}

		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, N*> graph(I first, S last) {
			std::transform(first, last, std::inserter(nodes_, std::begin(nodes_)), [](N x) {
				return std::make_shared<N>(x);
			});
		}

		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, value_type*> graph(I first, S last) {
			std::transform(first, last, std::inserter(nodes_, std::begin(nodes_)), [](value_type x) {
				return std::make_shared<N>(x.from);
			});

			std::transform(first, last, std::inserter(nodes_, std::begin(nodes_)), [](value_type x) {
				return std::make_shared<N>(x.to);
			});
			std::transform(first, last, std::inserter(edges_, std::begin(edges_)), [this](value_type x) {
				return std::make_shared<edge>(edge{*nodes_.find(x.from), *nodes_.find(x.to), x.weight});
			});
		}

		graph(graph&& other) noexcept
		: nodes_{std::exchange(other.nodes_, std::set<std::shared_ptr<N>, nodes_comparator>())}
		, edges_{std::exchange(other.edges_, std::set<std::shared_ptr<edge>, edges_comparator>())} {}

		auto operator=(graph&& other) noexcept -> graph& {
			std::swap(nodes_, other.nodes_);
			std::swap(nodes_, other.nodes_);
			return *this;
		}

		graph(graph const& other) {
			std::transform(std::cbegin(other.nodes_),
			               std::cend(other.nodes_),
			               std::inserter(nodes_, std::begin(nodes_)),
			               [](std::shared_ptr<N> x) { return std::make_shared<N>(*x); });

			std::transform(std::cbegin(other.edges_),
			               std::cend(other.edges_),
			               std::inserter(edges_, std::begin(edges_)),
			               [this](std::shared_ptr<edge> x) {
				               return std::make_shared<edge>(
				                  edge{*nodes_.find(*(x->from)), *nodes_.find(*(x->to)), x->weight});
			               });
		}

		auto operator=(graph const& other) -> graph& {
			if (*this == other) {
				return *this;
			}
			auto tmp = graph<N, E>(other);
			std::swap(nodes_, tmp.nodes_);
			std::swap(edges_, tmp.edges_);
			return *this;
		}
		// Your member functions go here

		auto insert_node(N const& value) -> bool {
			if (is_node(value)) {
				return false;
			}
			return nodes_.emplace(std::make_shared<N>(value)).second;
		}

		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (is_node(src) && is_node(dst)) {
				return edges_
				   .emplace(std::make_shared<edge>(edge{*nodes_.find(src), *nodes_.find(dst), weight}))
				   .second;
			}
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
			                         "or dst node does not exist");
		}

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (is_node(old_data)) {
				if (is_node(new_data)) {
					return false;
				}
				auto old_node = nodes_.find(old_data);
				auto new_node = std::make_shared<N>(new_data);
				nodes_.emplace(new_node);

				auto to_change = std::find_if(
				   std::cbegin(edges_),
				   std::cend(edges_),
				   [old_data](std::shared_ptr<edge> x) { return (*(x->from) == old_data); });

				while (to_change != std::cend(edges_)) {
					insert_edge(*new_node, *((*to_change)->to), (*to_change)->weight);
					edges_.erase(to_change);
					to_change = std::find_if(
					   std::cbegin(edges_),
					   std::cend(edges_),
					   [old_data](std::shared_ptr<edge> x) { return (*(x->from) == old_data); });
				}
				to_change =
				   std::find_if(std::cbegin(edges_),
				                std::cend(edges_),
				                [old_data](std::shared_ptr<edge> x) { return (*(x->to) == old_data); });
				while (to_change != std::cend(edges_)) {
					insert_edge(*((*to_change)->from), *new_node, (*to_change)->weight);
					edges_.erase(to_change);
					to_change = std::find_if(
					   std::cbegin(edges_),
					   std::cend(edges_),
					   [old_data](std::shared_ptr<edge> x) { return (*(x->to) == old_data); });
				}
				nodes_.erase(old_node);
				return true;
			}
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
			                         "doesn't exist");
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (is_node(old_data) and is_node(new_data)) {
				auto old_node = nodes_.find(old_data);

				auto to_change = std::find_if(
				   std::cbegin(edges_),
				   std::cend(edges_),
				   [old_data](std::shared_ptr<edge> x) { return (*(x->from) == old_data); });

				while (to_change != std::cend(edges_)) {
					insert_edge(new_data, *((*to_change)->to), (*to_change)->weight);
					edges_.erase(to_change);
					to_change = std::find_if(
					   std::cbegin(edges_),
					   std::cend(edges_),
					   [old_data](std::shared_ptr<edge> x) { return (*(x->from) == old_data); });
				}

				to_change =
				   std::find_if(std::cbegin(edges_),
				                std::cend(edges_),
				                [old_data](std::shared_ptr<edge> x) { return (*(x->to) == old_data); });
				while (to_change != std::cend(edges_)) {
					insert_edge(*((*to_change)->from), new_data, (*to_change)->weight);
					edges_.erase(to_change);
					to_change = std::find_if(
					   std::cbegin(edges_),
					   std::cend(edges_),
					   [old_data](std::shared_ptr<edge> x) { return (*(x->to) == old_data); });
				}
				nodes_.erase(old_node);
			}
			else {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or "
				                         "new data if they don't exist in the graph");
			}
		}

		auto erase_node(N const& value) -> bool {
			if (is_node(value)) {
				std::erase_if(edges_, [value](std::shared_ptr<edge> x) {
					return (*(x->from) == value or *(x->to) == value);
				});
				nodes_.erase(nodes_.find(value));

				return true;
			}
			return false;
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (is_node(src) and is_node(dst)) {
				auto erased = std::erase_if(edges_, [src, dst, weight](std::shared_ptr<edge> x) {
					return (*(x->from) == src and *(x->to) == dst and x->weight == weight);
				});
				return (erased != 0);
			}
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they "
			                         "don't exist in the graph");
		}

		auto erase_edge(iterator i) -> iterator {
			i.graph_it_ = edges_.erase(i.graph_it_);
			return i;
		}

		auto erase_edge(iterator i, iterator s) -> iterator {
			i.graph_it_ = edges_.erase(i.graph_it_, s.graph_it_);
			return i;
		}

		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
		}

		// accessors
		[[nodiscard]] auto is_node(N const& value) const -> bool {
			return (nodes_.find(value) != std::cend(nodes_));
		}

		[[nodiscard]] auto empty() const -> bool {
			return nodes_.empty();
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (is_node(src) and is_node(dst)) {
				auto connection = std::find_if(std::cbegin(edges_),
				                               std::cend(edges_),
				                               [src, dst](std::shared_ptr<edge> x) {
					                               return (*(x->from) == src and *(x->to) == dst);
				                               });
				return connection != std::cend(edges_);
			}
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
			                         "don't exist in the graph");
		}

		[[nodiscard]] auto nodes() const -> std::vector<N> {
			auto result = std::vector<N>();
			std::transform(std::cbegin(nodes_),
			               std::cend(nodes_),
			               std::back_inserter(result),
			               [](std::shared_ptr<N> x) { return *x; });
			return result;
		}

		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E> {
			if (is_node(src) and is_node(dst)) {
				auto tmp = std::vector<std::shared_ptr<edge>>();

				std::copy_if(std::cbegin(edges_),
				             std::cend(edges_),
				             std::back_inserter(tmp),
				             [src, dst](std::shared_ptr<edge> x) {
					             return *(x->from) == src and *(x->to) == dst;
				             });

				auto result = std::vector<E>();
				std::transform(std::cbegin(tmp),
				               std::cend(tmp),
				               std::back_inserter(result),
				               [](std::shared_ptr<edge> x) { return x->weight; });
				return result;
			}

			throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
			                         "don't exist in the graph");
		}

		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const -> iterator {
			auto g_it = iterator();
			g_it.graph_it_ = std::find_if(std::cbegin(edges_),
			                              std::cend(edges_),
			                              [src, dst, weight](std::shared_ptr<edge> x) {
				                              return *(x->from) == src and *(x->to) == dst
				                                     and x->weight == weight;
			                              });
			return g_it;
		}

		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (is_node(src)) {
				auto tmp = std::vector<std::shared_ptr<edge>>();

				std::copy_if(std::cbegin(edges_),
				             std::cend(edges_),
				             std::back_inserter(tmp),
				             [src](std::shared_ptr<edge> x) { return *(x->from) == src; });

				auto result = std::vector<N>();
				std::transform(std::cbegin(tmp),
				               std::cend(tmp),
				               std::back_inserter(result),
				               [](std::shared_ptr<edge> x) { return *(x->to); });
				return result;
			}
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist "
			                         "in the graph");
		}

		[[nodiscard]] auto begin() const -> iterator {
			return iterator(std::begin(edges_));
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator(std::end(edges_));
		}

		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			if (std::size(other.nodes_) == std::size(nodes_)
			    and std::size(other.edges_) == std::size(edges_)) {
				auto node_equality = std::equal(
				   std::cbegin(nodes_),
				   std::cend(nodes_),
				   std::cbegin(other.nodes_),
				   [](std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) { return *a == *b; });

				return node_equality
				       and std::equal(std::cbegin(edges_),
				                      std::cend(edges_),
				                      std::cbegin(other.edges_),
				                      [](std::shared_ptr<edge> const& a, std::shared_ptr<edge> const& b) {
					                      return *(a->from) == *(b->from) and *(a->to) == *(b->to)
					                             and a->weight == b->weight;
				                      });
			}
			return false;
		}

		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			std::for_each(std::cbegin(g.nodes_), std::cend(g.nodes_), [&os, g](std::shared_ptr<N> x) {
				os << *x << " (\n";
				auto to_print = std::find_if(std::cbegin(g.edges_),
				                             std::cend(g.edges_),
				                             [x](std::shared_ptr<edge> y) { return (*(y->from) == *x); });

				while (to_print != std::cend(g.edges_)) {
					os << "  " << *((*to_print)->to) << " | " << (*to_print)->weight << "\n";
					++to_print;
					to_print = std::find_if(to_print, std::cend(g.edges_), [x](std::shared_ptr<edge> y) {
						return (*(y->from) == *x);
					});
				}
				os << ")\n";
			});

			return os;
		}

	private:
		struct nodes_comparator {
			using is_transparent = void;

			auto operator()(std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) const noexcept
			   -> bool {
				return *a < *b;
			}

			auto operator()(std::shared_ptr<N> const& a, N const& b) const noexcept -> bool {
				return *a < b;
			}

			auto operator()(N const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
				return a < *b;
			}
		};

		struct edges_comparator {
			using is_transparent = void;
			auto operator()(std::shared_ptr<edge> const& a,
			                std::shared_ptr<edge> const& b) const noexcept -> bool {
				if (*(a->from) != *(b->from)) {
					return *(a->from) < *(b->from);
				}

				if (*(a->to) != *(b->to)) {
					return *(a->to) < *(b->to);
				}

				return a->weight < b->weight;
			}
		};

		std::set<std::shared_ptr<N>, nodes_comparator> nodes_;
		std::set<std::shared_ptr<edge>, edges_comparator> edges_;

	public:
		class iterator {
			using graph_iterator = typename std::set<std::shared_ptr<edge>, edges_comparator>::iterator;

		public:
			using value_type = ranges::common_tuple<N, N, E>;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			// Iterator constructor
			iterator() = default;

			// Iterator source
			auto operator*() const -> ranges::common_tuple<N const&, N const&, E const&> {
				return ranges::common_tuple<N const&, N const&, E const&>(*((*graph_it_)->from),
				                                                          *((*graph_it_)->to),
				                                                          (*graph_it_)->weight);
			}
			// Iterator traversal
			auto operator++() -> iterator& {
				++graph_it_;
				return *this;
			}
			auto operator++(int) -> iterator {
				auto tmp = *this;
				++*this;
				return tmp;
			}

			auto operator--() -> iterator& {
				--graph_it_;
				return *this;
			}
			auto operator--(int) -> iterator {
				auto tmp = *this;
				--*this;
				return tmp;
			}

			// Iterator comparison
			auto operator==(iterator const& other) const -> bool = default;

		private:
			friend class graph<N, E>;
			explicit iterator(graph_iterator it)
			: graph_it_{it} {}
			graph_iterator graph_it_;
		};
	};

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
