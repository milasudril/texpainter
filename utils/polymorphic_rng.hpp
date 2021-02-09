//@	{
//@	 "targets":[{"name":"polymorphic_rng.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_POLYMORPHICRNG_HPP
#define TEXPAINTER_UTILS_POLYMORPHICRNG_HPP

#include <cstddef>
#include <type_traits>
#include <functional>

namespace Texpainter
{
	class PolymorphicRng
	{
	public:
		using result_type = size_t;

		template<class Rng>
		constexpr explicit PolymorphicRng(std::reference_wrapper<Rng> rng)
		    : m_state{&rng.get()}
		    , m_callback{[](void* val) {
			    return static_cast<result_type>((*static_cast<Rng*>(val))());
		    }}
		    , m_min{rng.get().min()}
		    , m_max{rng.get().max()}
		{
		}

		decltype(auto) operator()() { return m_callback(m_state); }

		result_type min() const { return m_min; }

		result_type max() const { return m_max; }


	private:
		void* m_state;
		size_t (*m_callback)(void*);
		result_type m_min;
		result_type m_max;
	};

}

#endif