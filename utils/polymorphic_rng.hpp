//@	{
//@	 "targets":[{"name":"polymorphic_rng.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_POLYMORPHICRNG_HPP
#define TEXPAINTER_UTILS_POLYMORPHICRNG_HPP

#include "./function_ref.hpp"

#include <cstddef>
#include <type_traits>

namespace Texpainter
{
	class PolymorphicRng
	{
	public:
		using result_type = size_t;

		template<
		   class Rng,
		   std::enable_if_t<
		      std::is_unsigned_v<
		         typename Rng::result_type> && sizeof(typename Rng::result_type) <= sizeof(result_type)
		         && !std::is_same_v<std::decay_t<Rng>, PolymorphicRng>,
		      int> = 0>
		explicit PolymorphicRng(Rng& rng): m_rng_state{rng}, m_min{rng.min()}, m_max{rng.max()}
		{
		}

		decltype(auto) operator()()
		{
			return m_rng_state();
		}

		result_type min() const
		{
			return m_min;
		}

		result_type max() const
		{
			return m_max;
		}


	private:
		FunctionRef<size_t()> m_rng_state;
		result_type m_min;
		result_type m_max;
	};

}

#endif