//@	{
//@	 "targets":[{"name":"box.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"box.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_BOX_HPP
#define TEXPAINTER_UI_BOX_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Box: public Container
	{
	public:
		static constexpr unsigned short Fill = 1;
		static constexpr unsigned short Expand = 2;
		static constexpr unsigned short PositionBack = 4;
		struct InsertMode
		{
			unsigned short padding;
			unsigned short flags;
		};

		enum class Orientation : int
		{
			Vertical,
			Horizontal
		};

		explicit Box(Container& parent, Orientation orientation, int global_spacing = 2);
		~Box() override;

		Box& operator=(Box&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Box(Box&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		Box& add(void* handle) override;
		Box& show() override;
		Box& sensitive(bool val) override;
		void* toplevel() const override;

		Box& homogenous(bool status) noexcept;
		Box& insertMode(const InsertMode& mode) noexcept;

		Box& alignment(float x) noexcept;

		Orientation orientation() const noexcept;

	protected:
		class Impl;
		explicit Box(Box::Impl& impl): m_impl(&impl)
		{
		}
		Impl* m_impl;
	};
}

#endif
