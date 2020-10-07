//@	{
//@	 "targets":[{"name":"maybe_owner.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_MAYBEOWNER_HPP
#define TEXPAINTER_UTILS_MAYBEOWNER_HPP

#include <functional>

#include <cassert>

namespace Texpainter
{
	template<class T>
	class MaybeOwner
	{
	public:
		explicit MaybeOwner(T&& data): r_data{nullptr}, m_data{std::move(data)} {}

		explicit MaybeOwner(std::reference_wrapper<T const> data): r_data{&data.get()} {}

		bool isOwner() const { return r_data == nullptr; }

		T const& get() const { return isOwner() ? m_data : *r_data; }

	private:
		T const* r_data;
		T m_data;
	};
}

#endif