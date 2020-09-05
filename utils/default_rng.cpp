//@	{
//@	  "targets":[{"name":"default_rng.o","type":"object"}]
//@	}

#include "./default_rng.hpp"
#include <exception>

namespace
{
	struct DummyRng
	{
		size_t operator()() const { std::terminate(); }

		static constexpr size_t min() { return 0; }

		static constexpr size_t max() { return 1; }

		using result_type = size_t;
	};

	constinit DummyRng s_dummy_rng;

	constinit Texpainter::PolymorphicRng s_default_rng{s_dummy_rng};
}

void Texpainter::DefaultRng::detail::engine(PolymorphicRng rng) { s_default_rng = rng; }

Texpainter::PolymorphicRng Texpainter::DefaultRng::engine() { return s_default_rng; }
