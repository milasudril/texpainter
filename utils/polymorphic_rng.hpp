//@	{
//@	 "targets":[{"name":"polymorphic_rng.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_POLYMORPHICRNG_HPP
#define TEXPAINTER_UTILS_POLYMORPHICRNG_HPP

#include <cstddef>
#include <type_traits>
#include <functional>
#include <climits>
#include <random>
#include <limits>

namespace Texpainter
{
	class PolymorphicRng
	{
	public:
		using result_type = size_t;

		template<class Rng>
		using RngType =
		    std::independent_bits_engine<Rng, sizeof(result_type) * CHAR_BIT, result_type>;

		template<class Rng>
		constexpr explicit PolymorphicRng(std::reference_wrapper<Rng> rng)
		    : m_state{&rng.get()}
		    , m_callback{
		          [](void* val) { return static_cast<result_type>((*static_cast<Rng*>(val))()); }}
		{
		}

		decltype(auto) operator()() { return m_callback(m_state); }

		static constexpr result_type min() { return 0; }

		static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

	private:
		void* m_state;
		size_t (*m_callback)(void*);
	};

}

#endif