//@	{
//@	"targets":[{"name":"fixed_flatmap.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_FIXEDFLATMAP_HPP
#define TEXPAINTER_UTILS_FIXEDFLATMAP_HPP

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
		using key_type    = std::decay_t<decltype(Keys::items[0])>;
		using mapped_type = Value;

		FixedFlatmap() = default;

		explicit FixedFlatmap(std::array<mapped_type, std::size(Keys::items)>&& init): m_vals{init}
		{
		}

		static constexpr auto size() { return std::size(Keys::items); }

		static constexpr auto const& keys() { return s_keys; }

		constexpr auto const& values() const { return m_vals; }

		constexpr auto& values() { return m_vals; }

		template<class KeyLike>
		static constexpr auto keyIndex(KeyLike const& key)
		{
			auto i = std::lower_bound(std::begin(s_keys), std::end(s_keys), key, Compare{});
			if(i != std::end(s_keys) && !Compare{}(key, *i)) [[likely]]
				{
					return static_cast<size_t>(i - std::begin(s_keys));
				}

			return std::numeric_limits<size_t>::max();
		}

		template<auto key>
		constexpr auto find() const
		{
			constexpr auto i = keyIndex(key);
			static_assert(i != std::numeric_limits<size_t>::max(), "Key not found");
			return std::begin(m_vals) + i;
		}

		template<auto key>
		constexpr auto find()
		{
			return const_cast<mapped_type*>(std::as_const(*this).template find<key>());
		}

		constexpr auto find(key_type const& key) const
		{
			if(auto i = keyIndex(key); i != std::numeric_limits<size_t>::max()) [[likely]]
				{
					return std::begin(m_vals) + i;
				}
			return static_cast<mapped_type const*>(nullptr);
		}

		constexpr auto find(key_type const& key)
		{
			return const_cast<mapped_type*>(std::as_const(*this).find(key));
		}

	private:
		static constexpr auto s_keys = fixed_flatmap_detail::sort(Keys::items, Compare{});
		std::array<mapped_type, size()> m_vals;
	};


	template<size_t N>
	class Str
	{
	public:
		constexpr Str(char const (&val)[N]): value{} { std::ranges::copy(val, value); }

		constexpr operator std::string_view() const { return value; }

		constexpr char const* c_str() const { return value; }

		char value[N];
	};
}

#endif