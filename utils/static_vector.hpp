//@	{"targets":[{"name":"static_vector.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_STATICVECTOR_HPP
#define TEXPAINTER_UTILS_STATICVECTOR_HPP

#include <array>

namespace Texpainter
{
	template<class T, size_t N>
	class StaticVector
	{
	public:
		static constexpr auto capacity()
		{ return N; }

		void push_back(T&& val)
		{
			m_data[m_size] = std::move(val);
			++m_size;
		}

		auto size() const { return std::size(m_data); }

		auto begin() const { return std::begin(m_data); }
		auto end() const { return std::end(m_data); }
		auto data() const { return std::data(m_data); }

		auto begin() { return std::begin(m_data); }
		auto end() { return std::end(m_data); }
		auto data() { return std::data(m_data); }

	private:
		size_t m_size;
		std::array<T, N> m_data;
	};
}

#endif