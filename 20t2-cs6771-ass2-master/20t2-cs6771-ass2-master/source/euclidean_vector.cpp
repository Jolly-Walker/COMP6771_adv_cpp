// Copyright (c) Christopher Di Bella.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
#include "comp6771/euclidean_vector.hpp"
#include "gsl-lite/gsl-lite.hpp"
#include <algorithm>
#include <bits/types/FILE.h>
#include <cstddef>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <span>
#include <utility>
#include <vector>

namespace comp6771 {

	euclidean_vector::euclidean_vector()
	: euclidean_vector(1, 0.0) {}
	euclidean_vector::euclidean_vector(int dimensions)
	: euclidean_vector(dimensions, 0.0) {}

	euclidean_vector::euclidean_vector(int dimensions, double magnitude)
	: dimensions_{dimensions}
	, cache_{-1} {
		if (dimensions_ == 0) {
			magnitudes_ = nullptr;
		}
		else {
			auto size = gsl_lite::narrow_cast<std::vector<double>::size_type>(dimensions);
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitudes_ = std::make_unique<double[]>(size);
			span_ = std::span<double>(magnitudes_.get(), size);
			std::fill(span_.begin(), span_.end(), magnitude);
		}
	}

	euclidean_vector::euclidean_vector(std::vector<double>::const_iterator start,
	                                   std::vector<double>::const_iterator end)
	: cache_{-1} {
		dimensions_ = gsl_lite::narrow_cast<int>(ranges::distance(start, end));

		if (dimensions_ == 0) {
			magnitudes_ = nullptr;
		}
		else {
			auto const size = gsl_lite::narrow_cast<std::vector<double>::size_type>(dimensions_);
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitudes_ = std::make_unique<double[]>(size);
			span_ = std::span<double>(magnitudes_.get(), size);
			std::transform(start, end, span_.begin(), [](double x) -> double { return x; });
		}
	}
	euclidean_vector::euclidean_vector(euclidean_vector const& a)
	: dimensions_{a.dimensions_}
	, cache_{-1} {
		if (dimensions_ == 0) {
			magnitudes_ = nullptr;
		}
		else {
			auto const size = gsl_lite::narrow_cast<std::vector<double>::size_type>(dimensions_);
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitudes_ = std::make_unique<double[]>(size);
			span_ = std::span<double>(magnitudes_.get(), size);
			std::transform(a.span_.begin(), a.span_.end(), span_.begin(), [](double x) -> double {
				return x;
			});
		}
	}

	euclidean_vector::euclidean_vector(std::initializer_list<double> list)
	: dimensions_{gsl_lite::narrow_cast<int>(ranges::distance(list))}
	, cache_{-1} {
		if (dimensions_ == 0) {
			magnitudes_ = nullptr;
		}
		else {
			auto const size = gsl_lite::narrow_cast<std::vector<double>::size_type>(dimensions_);
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitudes_ = std::make_unique<double[]>(size);
			span_ = std::span<double>(magnitudes_.get(), size);
			std::transform(list.begin(), list.end(), span_.begin(), [](double x) -> double { return x; });
		}
	}

	euclidean_vector::euclidean_vector(euclidean_vector&& a) noexcept
	: dimensions_{std::exchange(a.dimensions_, 0)}
	, magnitudes_{std::exchange(a.magnitudes_, nullptr)}
	, span_{std::exchange(a.span_, std::span<double>())}
	, cache_{std::exchange(a.cache_, -1)} {}

	auto euclidean_vector::operator=(euclidean_vector const& a) -> euclidean_vector& {
		auto copy = euclidean_vector(a);
		copy.swap(*this);
		return *this;
	}

	auto euclidean_vector::operator=(euclidean_vector&& ori) noexcept -> euclidean_vector& {
		dimensions_ = std::exchange(ori.dimensions_, 0);
		magnitudes_ = std::exchange(ori.magnitudes_, nullptr);
		span_ = std::exchange(ori.span_, std::span<double>());
		cache_ = std::exchange(ori.cache_, -1);
		return *this;
	}

	auto euclidean_vector::swap(euclidean_vector& a) -> void {
		std::swap(dimensions_, a.dimensions_);
		std::swap(magnitudes_, a.magnitudes_);
		std::swap(span_, a.span_);
		std::swap(cache_, a.cache_);
	}

	auto euclidean_vector::operator[](int i) noexcept -> double& {
		assert(i < dimensions_);
		cache_ = -1;
		return magnitudes_[gsl_lite::narrow_cast<std::size_t>(i)];
	}

	auto euclidean_vector::operator[](int i) const noexcept -> double {
		assert(i < dimensions_);
		return magnitudes_[gsl_lite::narrow_cast<std::size_t>(i)];
	}

	auto euclidean_vector::operator+() const -> euclidean_vector {
		return *this;
	}

	auto euclidean_vector::operator-() const -> euclidean_vector {
		euclidean_vector copy = *this;

		std::transform(copy.span_.begin(), copy.span_.end(), copy.span_.begin(), std::negate<>());
		return copy;
	}

	auto euclidean_vector::operator+=(euclidean_vector const& other) -> euclidean_vector& {
		if (other.dimensions_ != dimensions_) {
			throw euclidean_vector_error(fmt::format("Dimensions of LHS({}) and RHS({}) do not match",
			                                         dimensions_,
			                                         other.dimensions_));
		}
		cache_ = -1;
		std::transform(span_.begin(), span_.end(), other.span_.begin(), span_.begin(), std::plus<>());
		return *this;
	}

	auto euclidean_vector::operator-=(euclidean_vector const& other) -> euclidean_vector& {
		if (other.dimensions_ != this->dimensions_) {
			throw euclidean_vector_error(fmt::format("Dimensions of LHS({}) and RHS({}) do not match",
			                                         dimensions_,
			                                         other.dimensions_));
		}
		cache_ = -1;
		std::transform(span_.begin(), span_.end(), other.span_.begin(), span_.begin(), std::minus<>());
		return *this;
	}

	auto euclidean_vector::operator*=(double d) noexcept -> euclidean_vector& {
		std::transform(span_.begin(), span_.end(), span_.begin(), [d](double x) -> double {
			return x * d;
		});
		cache_ = -1;
		return *this;
	}

	auto euclidean_vector::operator/=(double d) -> euclidean_vector& {
		if (d == 0) {
			throw euclidean_vector_error("Invalid vector division by 0");
		}
		std::transform(span_.begin(), span_.end(), span_.begin(), [d](double x) -> double {
			return x / d;
		});
		cache_ = -1;
		return *this;
	}

	euclidean_vector::operator std::vector<double>() const {
		auto result = std::vector<double>(gsl_lite::narrow_cast<std::size_t>(dimensions_));

		std::transform(span_.begin(), span_.end(), result.begin(), [](double x) -> double { return x; });

		return result;
	}

	euclidean_vector::operator std::list<double>() const {
		auto result = std::list<double>(gsl_lite::narrow_cast<std::size_t>(dimensions_));

		std::transform(span_.begin(), span_.end(), result.begin(), [](double x) -> double { return x; });

		return result;
	}

	auto euclidean_vector::at(int i) const -> double {
		if (i < 0 or i >= dimensions_) {
			throw euclidean_vector_error(
			   fmt::format("Index {} is not valid for this euclidean_vector object", i));
		}
		return this->magnitudes_[gsl_lite::narrow_cast<std::size_t>(i)];
	}

	auto euclidean_vector::at(int i) -> double& {
		if (i < 0 or i >= dimensions_) {
			throw euclidean_vector_error(
			   fmt::format("Index {} is not valid for this euclidean_vector object", i));
		}
		cache_ = -1;
		return magnitudes_[gsl_lite::narrow_cast<std::size_t>(i)];
	}

	auto euclidean_vector::dimensions() const noexcept -> int {
		return dimensions_;
	}

	auto euclidean_norm(euclidean_vector const& v) -> double {
		if (v.dimensions() == 0) {
			throw euclidean_vector_error("euclidean_vector with no dimensions does not have a "
			                             "norm");
		}
		return inner_norm(v);
	}

	auto unit(euclidean_vector const& v) -> euclidean_vector {
		if (v.dimensions() == 0) {
			throw euclidean_vector_error("euclidean_vector with no dimensions does not have a unit "
			                             "vector");
		}
		auto copy = v;
		auto norm = euclidean_norm(v);
		if (norm == 0) {
			throw euclidean_vector_error("euclidean_vector with zero euclidean normal does not have a "
			                             "unit vector");
		}
		copy /= norm;
		return copy;
	}

	auto dot(euclidean_vector const& x, euclidean_vector const& y) -> double {
		if (x.dimensions() != y.dimensions()) {
			throw euclidean_vector_error(fmt::format("Dimensions of LHS({}) and RHS({}) do not match",
			                                         x.dimensions(),
			                                         y.dimensions()));
		}
		return inner_dot(x, y);
	}
} // namespace comp6771
