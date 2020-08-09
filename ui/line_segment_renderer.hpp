//@	{
//@	 "targets":[{"name":"line_segment_renderer.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_LINESEGMENTRENDERER_HPP
#define TEXPAINTER_UI_LINESEGMENTRENDERER_HPP

#include "./toplevel_coordinates.hpp"

namespace Texpainter::Ui
{
	class LineSegmentRenderer
	{
	public:
		explicit LineSegmentRenderer(Container& container);
		~LineSegmentRenderer();

		LineSegmentRenderer& operator=(LineSegmentRenderer&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		LineSegmentRenderer& lineSegments(std::span<std::pair<ToplevelCoordinats, ToplevelCoordinates> const>);

	protected:
		class Impl;
		Impl* m_impl;

		explicit LineSegmentRenderer(Impl& impl): m_impl(&impl) {}
	};
}

#endif