//@	{
//@	"targets":[{"name":"fixed_flatmap.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FIXEDFLATMAP_HPP
#define TEXPAINTER_FIXEDFLATMAP_HPP

#include <algorithm>
#include <array>
#include <type_traits>

namespace Texpainter
{
	namespace fixed_flatmap_detail
	{
		template<class T, size_t N, class Compare>
		constexpr auto sort(std::array<T, N> const& x, Compare const& compare)
		{
			auto tmp = x;
			std::ranges::sort(tmp, compare);
			return tmp;
		}
	}

	template<class Keys, class Value, class Compare = std::less<decltype(Keys::items[0])>>
	class FixedFlatmap
	{
	public:
		using key_type   = std::remove_reference_t<decltype(Keys::items[0])>;
		using value_type = Value;

		static constexpr auto size() { return std::size(Keys::items); }

		static constexpr auto const keys() { return s_keys; }

		constexpr auto const values() const { return m_vals; }

		constexpr auto values() { return m_vals; }

		constexpr auto find(key_type const& key) const
		{
			auto i = std::ranges::lower_bound(s_keys, key, Compare{});
			if(i != std::end(s_keys) && !Compare{}(key, *i)) [[likely]]
				{
					return std::begin(m_vals) + (i - std::begin(s_keys));
				}

			return static_cast<value_type const*>(nullptr);
		}

		constexpr auto find(key_type const& key)
		{
			return const_cast<value_type*>(std::as_const(*this).find(key));
		}

	private:
		static constexpr auto s_keys = fixed_flatmap_detail::sort(Keys::items, Compare{});
		std::array<value_type, size()> m_vals;
	};
}

#endif