//@	{
//@  "targets":[{"name":"separator.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"separator.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_SEPARATOR_HPP
#define TEXPAINTER_SEPARATOR_HPP

#include "./container.hpp"
#include "./box.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Separator
	{
	public:
		explicit Separator(Box& box)
		    : Separator{box, box.orientation() == Box::Orientation::Horizontal}
		{
		}

		explicit Separator(Container& container, bool vertical) noexcept;
		~Separator();

		Separator& operator=(Separator&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Separator(Separator&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

	protected:
		class Impl;
		Impl* m_impl;
		explicit Separator(Impl& impl): m_impl(&impl) {}
	};
}

#endif
