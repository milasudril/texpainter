//@	{
//@	 "targets":[{"name":"filler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filler.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_FILLER_HPP
#define TEXPAINTER_UI_FILLER_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Filler
	{
	public:
		explicit Filler(Container& cnt);
		~Filler();

		Filler& operator=(Filler&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Filler(Filler&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

	private:
		class Impl;
		Impl* m_impl;

		Filler(Impl& impl): m_impl(&impl) {}
	};
}

#endif