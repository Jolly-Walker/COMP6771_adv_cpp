#include "comp6771/euclidean_vector.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <limits>
#include <type_traits>
#include <vector>

/*
   This block of tests check if constructors are constructed properly,
   It also checks for the correctness of dimensions() and operator[]
*/
TEST_CASE("constructors") {
	SECTION("default") {
		auto a = comp6771::euclidean_vector();
		CHECK(a.dimensions() == 1);
		CHECK(a[0] == 0);

		auto const b = comp6771::euclidean_vector();
		CHECK(b.dimensions() == 1);
		CHECK(b[0] == 0);
	}

	SECTION("single") {
		auto a = comp6771::euclidean_vector(3);
		CHECK(a.dimensions() == 3);
		CHECK(a[0] == 0);
		CHECK(a[1] == 0);
		CHECK(a[2] == 0);

		auto const b = comp6771::euclidean_vector(3);
		CHECK(b.dimensions() == 3);
		CHECK(b[0] == 0);
		CHECK(b[1] == 0);
		CHECK(b[2] == 0);
	}

	SECTION("standard") {
		auto a = comp6771::euclidean_vector(2, 1.5);

		CHECK(a[0] == 1.5);
		CHECK(a[1] == 1.5);
		CHECK(a.dimensions() == 2);

		auto const b = comp6771::euclidean_vector(3, -1.5);

		CHECK(b[0] == -1.5);
		CHECK(b[1] == -1.5);
		CHECK(b[2] == -1.5);
		CHECK(b.dimensions() == 3);
	}

	SECTION("iterators") {
		auto v = std::vector<double>{1, 2, 3, 4};
		auto a = comp6771::euclidean_vector(v.begin(), v.end());
		CHECK(a.dimensions() == ranges::distance(v));
		CHECK(a[0] == 1);
		CHECK(a[1] == 2);
		CHECK(a[2] == 3);
		CHECK(a[3] == 4);

		auto const b = comp6771::euclidean_vector(v.begin(), v.end());
		CHECK(b.dimensions() == ranges::distance(v));
		CHECK(b[0] == 1);
		CHECK(b[1] == 2);
		CHECK(b[2] == 3);
		CHECK(b[3] == 4);
	}

	SECTION("init list") {
		auto a = comp6771::euclidean_vector{1, 2};
		CHECK(a.dimensions() == 2);
		CHECK(a[0] == 1);
		CHECK(a[1] == 2);

		auto const b = comp6771::euclidean_vector{2, 3, 4};
		CHECK(b.dimensions() == 3);
		CHECK(b[0] == 2);
		CHECK(b[1] == 3);
		CHECK(b[2] == 4);
	}

	SECTION("copy") {
		auto a = comp6771::euclidean_vector(2);
		auto b = a;
		CHECK(b.dimensions() == 2);
		CHECK(b[0] == 0);
		CHECK(b[1] == 0);

		auto const c = comp6771::euclidean_vector(2, 3);
		b = c;
		CHECK(b.dimensions() == 2);
		CHECK(b[0] == 3);
		CHECK(b[1] == 3);
	}

	SECTION("move") {
		auto a = comp6771::euclidean_vector(2, 2);
		auto b = std::move(a);
		CHECK(b.dimensions() == 2);
		CHECK(b[0] == 2);
		CHECK(b[1] == 2);
	}
}

/*
   This section checks the == and != implementations first
   as we will use them to test the operations
*/

TEST_CASE("== and != friends") {
	SECTION("==") {
		auto const a = comp6771::euclidean_vector{1, 2, 3};
		auto const b = comp6771::euclidean_vector{1, 2, 3};
		auto const c = comp6771::euclidean_vector{1, -2, 3};
		auto const d = comp6771::euclidean_vector(2);
		CHECK(a == b);
		CHECK(b == a);
		CHECK_FALSE(a == c);
		CHECK_FALSE(a == d);
	}

	SECTION("!=") {
		auto const a = comp6771::euclidean_vector{1, 2, 3};
		auto const b = comp6771::euclidean_vector{1, 2, 3};
		auto const c = comp6771::euclidean_vector{1, -2, 3};
		auto const d = comp6771::euclidean_vector(2);
		CHECK(a != c);
		CHECK(c != a);
		CHECK_FALSE(a != b);
		CHECK(a != d);
	}
}

/*
   Here we test the operations, with the assumptions
   that operator== is implemented correctly
*/
TEST_CASE("Operation") {
	SECTION("copy assignment") {
		auto const a = comp6771::euclidean_vector(1, 1);
		auto b = comp6771::euclidean_vector(2, 2);
		CHECK(a.dimensions() == 1);
		CHECK(a[0] == 1);
		CHECK(b.dimensions() == 2);
		CHECK(b[0] == 2);
		CHECK(b[1] == 2);
		b = a;
		CHECK(b.dimensions() == 1);
		CHECK(b[0] == 1);
	}

	SECTION("move assignment") {
		auto a = comp6771::euclidean_vector(1, 1);
		auto b = comp6771::euclidean_vector(2, 2);
		CHECK(b.dimensions() == 2);
		CHECK(b[0] == 2);
		CHECK(b[1] == 2);
		b = std::move(a);
		CHECK(b.dimensions() == 1);
		CHECK(b[0] == 1);
	}

	SECTION("subscript") {
		auto const a = comp6771::euclidean_vector{1, 5, 8, 1};
		CHECK(a[0] == 1);
		CHECK(a[1] == 5);
		CHECK(a[2] == 8);
		CHECK(a[3] == 1);

		auto b = comp6771::euclidean_vector{-1.2, 5};
		CHECK(b[0] == -1.2);
		CHECK(b[1] == 5);
		b[1] = 20.2;
		CHECK(b[1] == 20.2);
	}

	SECTION("unary plus") {
		auto const a = comp6771::euclidean_vector{1, -5, 8, 1};
		CHECK(+a == comp6771::euclidean_vector{1, -5, 8, 1});
	}

	SECTION("negation") {
		auto const a = comp6771::euclidean_vector{1, -5, 8, 1};
		CHECK(-a == comp6771::euclidean_vector{-1, 5, -8, -1});
	}

	SECTION("+=") {
		auto a = comp6771::euclidean_vector{1, -5, 8, 1};
		auto const b = comp6771::euclidean_vector{-1, 5, -8, -1};
		a += b;
		CHECK(a == comp6771::euclidean_vector(4));
		auto c = comp6771::euclidean_vector();
		CHECK_THROWS_MATCHES(a += c,
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Dimensions of LHS(4) and RHS(1) do not "
		                                              "match"));
	}

	SECTION("-=") {
		auto a = comp6771::euclidean_vector{1, 2, 3, 4};
		auto const b = comp6771::euclidean_vector{0, 1, 2, 3};
		a -= b;
		CHECK(a == comp6771::euclidean_vector{1, 1, 1, 1});
		auto c = comp6771::euclidean_vector();
		CHECK_THROWS_MATCHES(a += c,
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Dimensions of LHS(4) and RHS(1) do not "
		                                              "match"));
	}

	SECTION("*=") {
		auto a = comp6771::euclidean_vector{2, 4, 5.5, -4};
		a *= 2.0;
		CHECK(a == comp6771::euclidean_vector{4, 8, 11, -8});
	}

	SECTION("/=") {
		auto a = comp6771::euclidean_vector{2, 4, 6, 8};
		a /= 2.0;
		CHECK(a == comp6771::euclidean_vector{1, 2, 3, 4});
	}

	SECTION("vector type conversion") {
		auto const a = comp6771::euclidean_vector{2, 4.4, 6, 8, 10.1};
		auto v = static_cast<std::vector<double>>(a);
		CHECK(v == std::vector<double>{2, 4.4, 6, 8, 10.1});
	}

	SECTION("vector type conversion") {
		auto const a = comp6771::euclidean_vector{2, 4.4, 6, 8, 10.1};
		auto v = static_cast<std::list<double>>(a);
		CHECK(v == std::list<double>{2, 4.4, 6, 8, 10.1});
	}
}

// Here we test the .at() function, ensuring that it
// properly throws exceptions and non const .at() calls
// can mutate the magnitude values
TEST_CASE("at() tests") {
	SECTION("at part 1") {
		auto const a = comp6771::euclidean_vector{5, 4, 3, 2, 1};
		CHECK(a.at(0) == 5);
		CHECK(a.at(1) == 4);
		CHECK(a.at(2) == 3);
		CHECK(a.at(3) == 2);
		CHECK(a.at(4) == 1);
		CHECK_THROWS_MATCHES(a.at(5),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Index 5 is not valid for this "
		                                              "euclidean_vector object"));
		CHECK_THROWS_MATCHES(a.at(-2),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Index -2 is not valid for this "
		                                              "euclidean_vector object"));
	}

	SECTION("at part 2") {
		auto a = comp6771::euclidean_vector{5, 4, 3, 2, 1};
		CHECK(a.at(0) == 5);
		CHECK(a.at(1) == 4);
		CHECK(a.at(2) == 3);
		CHECK(a.at(3) == 2);
		CHECK(a.at(4) == 1);
		a.at(0) = 7.8;
		CHECK(a.at(0) == 7.8);
		CHECK_THROWS_MATCHES(a.at(6),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Index 6 is not valid for this "
		                                              "euclidean_vector object"));
		CHECK_THROWS_MATCHES(a.at(-1),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Index -1 is not valid for this "
		                                              "euclidean_vector object"));
	}
}
TEST_CASE("other friend tests") {
	SECTION("addition") {
		auto const a = comp6771::euclidean_vector{1, 2, 3};
		auto const b = comp6771::euclidean_vector{1, -2, 3};
		auto const c = b + a;
		CHECK(c == comp6771::euclidean_vector{2, 0, 6});
		CHECK(a == comp6771::euclidean_vector{1, 2, 3});
		CHECK(b == comp6771::euclidean_vector{1, -2, 3});

		auto const d = comp6771::euclidean_vector{1, -2, 3, 20.2};

		CHECK_THROWS_MATCHES(a + d,
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Dimensions of LHS(3) and RHS(4) do not "
		                                              "match"));
	}

	SECTION("subtraction") {
		auto const a = comp6771::euclidean_vector{1, 2, 3};
		auto const b = comp6771::euclidean_vector{1, -2, 3};
		auto const c = b - a;
		CHECK(c == comp6771::euclidean_vector{0, -4, 0});
		CHECK(a == comp6771::euclidean_vector{1, 2, 3});
		CHECK(b == comp6771::euclidean_vector{1, -2, 3});

		auto const d = comp6771::euclidean_vector{1, -2, 3, 20.2};

		CHECK_THROWS_MATCHES(a - d,
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Dimensions of LHS(3) and RHS(4) do not "
		                                              "match"));
	}

	SECTION("multiply") {
		auto const a = comp6771::euclidean_vector{1, 2, 3};
		auto b = a * 1.1;
		Approx target_0 = Approx(b.at(0)).epsilon(std::numeric_limits<double>::epsilon());
		Approx target_1 = Approx(b.at(1)).epsilon(std::numeric_limits<double>::epsilon());
		Approx target_2 = Approx(b.at(2)).epsilon(std::numeric_limits<double>::epsilon());
		CHECK(target_0 == 1.1);
		CHECK(target_1 == 2.2);
		CHECK(target_2 == 3.3);
	}

	SECTION("division") {
		auto const a = comp6771::euclidean_vector{10, 20, 30.5};
		auto const b = a / 5;
		CHECK(b == comp6771::euclidean_vector{2, 4, 6.1});
	}

	SECTION("output stream") {
		auto const a = comp6771::euclidean_vector{10, -20, 30.5};
		CHECK(fmt::format("{}", a) == "[10 -20 30.5]");
	}
}

TEST_CASE("utility functions") {
	SECTION("euclidean_norm") {
		auto const a = comp6771::euclidean_vector{3, 4};
		auto const norm = comp6771::euclidean_norm(a);
		CHECK(norm == 5);

		auto const b = comp6771::euclidean_vector(0);

		CHECK_THROWS_MATCHES(comp6771::euclidean_norm(b),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("euclidean_vector with no dimensions does not "
		                                              "have a norm"));
	}

	SECTION("unit") {
		auto const a = comp6771::euclidean_vector{3, 4};
		auto const b = comp6771::unit(a);

		CHECK(b.at(0) == double(3.0 / 5.0));
		CHECK(b.at(1) == double(4.0 / 5.0));

		CHECK_THROWS_MATCHES(comp6771::unit(comp6771::euclidean_vector(0)),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("euclidean_vector with no dimensions does not "
		                                              "have a unit vector"));
		CHECK_THROWS_MATCHES(comp6771::unit(comp6771::euclidean_vector()),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("euclidean_vector with zero euclidean normal "
		                                              "does not have a unit vector"));
	}

	SECTION("dot product") {
		auto const a = comp6771::euclidean_vector{2, 1};
		auto const b = comp6771::euclidean_vector{8, 7};
		auto const c = comp6771::dot(a, b);
		CHECK(c == 23);
		CHECK_THROWS_MATCHES(comp6771::dot(a, comp6771::euclidean_vector(3)),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Dimensions of LHS(2) and RHS(3) do not "
		                                              "match"));
	}
}

/*
   This is to test miscellaneous stuff
   such as all operators not failing with
   the empty euclidean_vector and caching
   invalidation
*/
TEST_CASE("miscellaneous") {
	SECTION("empty") {
		auto a = comp6771::euclidean_vector(0);
		auto const b = comp6771::euclidean_vector(0, 10);
		CHECK(a.dimensions() == 0);
		CHECK(b.dimensions() == 0);
		auto a_str = fmt::format("{}", a);
		auto b_str = fmt::format("{}", b);
		CHECK(a_str == "[]");
		CHECK(a_str == b_str);
		CHECK(a == b);
		CHECK_NOTHROW(a + b);
		CHECK_NOTHROW(a - b);
		CHECK_NOTHROW(a += b);
		CHECK_NOTHROW(a -= b);
		CHECK_NOTHROW(a *= 5);
		CHECK_NOTHROW(a /= 5);
		CHECK_NOTHROW(b * 5);
		CHECK_NOTHROW(b / 5);
		auto vec = static_cast<std::vector<double>>(a);
		CHECK(vec == std::vector<double>(0));

		CHECK_THROWS_MATCHES(a.at(0),
		                     comp6771::euclidean_vector_error,
		                     Catch::Matchers::Message("Index 0 is not valid for this "
		                                              "euclidean_vector object"));
	}

	SECTION("cache invalidation") {
		auto a = comp6771::euclidean_vector{1, 2, 3};
		auto norm1 = comp6771::euclidean_norm(a);
		a *= 2;
		auto norm2 = comp6771::euclidean_norm(a);
		a /= 2;
		auto norm3 = comp6771::euclidean_norm(a);
		a += comp6771::euclidean_vector{1, 1, 1};
		auto norm4 = comp6771::euclidean_norm(a);
		a -= comp6771::euclidean_vector{1, 1, 1};
		auto norm5 = comp6771::euclidean_norm(a);
		a.at(2) = 5;
		auto norm6 = comp6771::euclidean_norm(a);
		a[2] = 3;
		auto norm7 = comp6771::euclidean_norm(a);

		CHECK(norm1 == sqrt(14));
		CHECK(norm2 == sqrt(56));
		CHECK(norm4 == sqrt(29));
		CHECK(norm6 == sqrt(30));
		CHECK(norm1 == norm3);
		CHECK(norm3 == norm5);
		CHECK(norm5 == norm7);
	}
}
