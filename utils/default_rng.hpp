//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP


#include "pcg-cpp/include/pcg_random.hpp"

namespace Texpainter::DefaultRng
{
	namespace detail
	{
		using RngImpl = pcg64;
		using StateType = pcg64::state_type;
	}

	class SeedValue
	{
	public:
		constexpr explicit SeedValue():m_value{}{}

		constexpr explicit SeedValue(detail::StateType val):m_value{val}{}

		constexpr auto value() const { return m_value; }

		constexpr bool operator==(SeedValue const& other) const = default;

		constexpr bool operator!=(SeedValue const& other) const = default;

	private:
		detail::StateType m_value;
	};

	class Engine:private detail::RngImpl
	{
		using Impl = detail::RngImpl;

	public:
		Engine():detail::RngImpl{}{}

		explicit Engine(SeedValue seed):Impl{seed.value()}{}

		Engine& seed(SeedValue seed)
		{
			Impl::seed(seed.value());
			return *this;
		}

		using Impl::operator();
		using Impl::min;
		using Impl::max;
	};

	Engine& engine();

	inline void seed(SeedValue val)
	{
		engine().seed(val);
	}

	inline SeedValue genSeed()
	{
		auto& rng = engine();
		auto const a = detail::StateType{rng()};
		auto const b = detail::StateType{rng()} << 64;

		return SeedValue{a | b};
	}
}

#endif