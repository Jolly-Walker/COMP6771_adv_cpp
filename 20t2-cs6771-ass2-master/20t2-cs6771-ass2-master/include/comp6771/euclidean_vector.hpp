#ifndef COMP6771_EUCLIDEAN_VECTOR_HPP
#define COMP6771_EUCLIDEAN_VECTOR_HPP

#include "gsl-lite/gsl-lite.hpp"
#include <compare>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <ostream>
#include <range/v3/algorithm.hpp>
#include <range/v3/iterator.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace comp6771 {
	class euclidean_vector_error : public std::runtime_error {
	public:
		explicit euclidean_vector_error(std::string const& what) noexcept
		: std::runtime_error(what) {}
	};

	class euclidean_vector {
	public:
		euclidean_vector();
		explicit euclidean_vector(int);
		euclidean_vector(int dimensions, double magnitude);
		euclidean_vector(std::vector<double>::const_iterator start,
		                 std::vector<double>::const_iterator end);
		euclidean_vector(std::initializer_list<double>);
		euclidean_vector(euclidean_vector const&);
		euclidean_vector(euclidean_vector&& a) noexcept;
		~euclidean_vector() = default;
		auto operator=(euclidean_vector const&) -> euclidean_vector&;
		auto operator=(euclidean_vector&&) noexcept -> euclidean_vector&;
		auto operator[](int i) noexcept -> double&;
		auto operator[](int i) const noexcept -> double;
		auto operator+() const -> euclidean_vector;
		auto operator-() const -> euclidean_vector;
		auto operator+=(euclidean_vector const&) -> euclidean_vector&;
		auto operator-=(euclidean_vector const&) -> euclidean_vector&;
		auto operator*=(double) noexcept -> euclidean_vector&;
		auto operator/=(double) -> euclidean_vector&;
		explicit operator std::vector<double>() const;
		explicit operator std::list<double>() const;
		[[nodiscard]] auto at(int) const -> double;
		auto at(int) -> double&;
		[[nodiscard]] auto dimensions() const noexcept -> int;
		friend auto operator==(euclidean_vector const& a, euclidean_vector const& b) -> bool {
			if (a.dimensions() != b.dimensions()) {
				return false;
			}

			return std::equal(a.span_.begin(),
			                  a.span_.end(),
			                  b.span_.begin(),
			                  [](double const& l, double const& r) -> bool {
				                  return (std::fabs(l - r) <= std::numeric_limits<double>::epsilon());
			                  });
		}

		friend auto operator!=(euclidean_vector const& a, euclidean_vector const& b) -> bool {
			return !(a == b);
		}

		friend auto operator+(euclidean_vector const& a, euclidean_vector const& b)
		   -> euclidean_vector {
			if (a.dimensions_ != b.dimensions_) {
				throw euclidean_vector_error(fmt::format("Dimensions of LHS({}) and RHS({}) do not "
				                                         "match",
				                                         a.dimensions_,
				                                         b.dimensions_));
			}
			auto c = a;
			c += b;
			return c;
		}

		friend auto operator-(euclidean_vector const& a, euclidean_vector const& b)
		   -> euclidean_vector {
			if (a.dimensions_ != b.dimensions_) {
				throw euclidean_vector_error(fmt::format("Dimensions of LHS({}) and RHS({}) do not "
				                                         "match",
				                                         a.dimensions_,
				                                         b.dimensions_));
			}
			auto c = a;
			c -= b;
			return c;
		}

		friend auto operator*(euclidean_vector const& v, double num) -> euclidean_vector {
			euclidean_vector temp = v;
			temp *= num;
			return temp;
		}

		friend auto operator*(double num, euclidean_vector const& v) -> euclidean_vector {
			euclidean_vector temp = v;
			temp *= num;
			return temp;
		}

		friend auto operator/(euclidean_vector const& v, double num) -> euclidean_vector {
			if (num == 0) {
				throw euclidean_vector_error("Invalid vector division by 0");
			}
			euclidean_vector temp = v;
			temp /= num;
			return temp;
		}

		friend auto operator<<(std::ostream& os, euclidean_vector const& v) -> std::ostream& {
			if (v.dimensions() == 0) {
				os << "[]";
				return os;
			}
			os << "[";
			std::for_each (v.span_.begin(), v.span_.end() - 1, [&os](double const x) { os << x << " "; });

			auto const tail = gsl_lite::narrow_cast<std::vector<double>::size_type>(v.dimensions() - 1);
			os << v.magnitudes_[tail] << "]";
			return os;
		}

		friend auto inner_norm(euclidean_vector const& v) -> double {
			if (v.cache_ >= 0) {
				return v.cache_;
			}
			auto res = sqrt(std::inner_product(v.span_.begin(), v.span_.end(), v.span_.begin(), 0.0));
			v.cache_ = res;
			return res;
		}

		friend auto inner_dot(euclidean_vector const& x, euclidean_vector const& y) -> double {
			auto const& span_x = x.span_;
			auto const& span_y = y.span_;

			return std::inner_product(span_x.begin(), span_x.end(), span_y.begin(), 0.0);
		}

	private:
		int dimensions_;
		// NOLINTNEXTLINE
		std::unique_ptr<double[]> magnitudes_;
		auto swap(euclidean_vector& a) -> void;
		std::span<double> span_;
		mutable double cache_;
	};

	auto euclidean_norm(euclidean_vector const& v) -> double;
	auto unit(euclidean_vector const& v) -> euclidean_vector;
	auto dot(euclidean_vector const& x, euclidean_vector const& y) -> double;
} // namespace comp6771
#endif // COMP6771_EUCLIDEAN_VECTOR_HPP
