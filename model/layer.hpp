//@	{
//@	 "targets":[{"name":"layer.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"layer.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_LAYER_HPP
#define TEXPAINTER_LAYER_HPP

#include "utils/angle.hpp"
#include "utils/rect.hpp"

#include "image.hpp"

#include <memory>

namespace Texpainter::Model
{
	class Layer
	{
	public:
		explicit Layer(Size2d image):
		   m_loc{0.0, 0.0},
		   m_rot{0},
		   m_scale{1.0, 1.0},
		   m_content{std::make_shared<Image>(image)}
		{
			std::ranges::fill(m_content->pixels(), Pixel{0.0, 0.0, 0.0, 0.0});
		}

		Layer(Layer const& other) = delete;

		~Layer() = default;

		Layer(Layer&&) = default;

		Layer& operator=(Layer&&) = default;

		Layer& operator=(Layer const& other) = delete;

		Layer linkedLayer() const
		{
			return Layer{m_loc, m_rot, m_scale, m_content};
		}

#if 0
		Layer clonedLayer() const
		{
			return Layer{m_loc, m_rot, m_scale, std::make_shared<Image>(*m_content)};
		}
#endif

		Image& content()
		{
			return *m_content;
		}

		Image const& content() const
		{
			return *m_content;
		}

		vec2_t location() const
		{
			return m_loc;
		}

		Layer& location(vec2_t loc)
		{
			m_loc = loc;
			return *this;
		}

		Angle rotation() const
		{
			return m_rot;
		}

		Layer& rotation(Angle rot)
		{
			m_rot = rot;
			return *this;
		}

		vec2_t scaleFactor() const
		{
			return m_scale;
		}

		Layer& scaleFactor(vec2_t val)
		{
			m_scale = val;
			return *this;
		}

	private:
		vec2_t m_loc;
		Angle m_rot;
		vec2_t m_scale;
		std::shared_ptr<Image> m_content;

		explicit Layer(vec2_t loc, Angle rot, vec2_t scale, std::shared_ptr<Image> const& content):
		   m_loc{loc},
		   m_rot{rot},
		   m_scale{scale},
		   m_content{content}
		{
		}
	};

	inline vec2_t axisAlignedBoundingBox(Layer const& layer)
	{
		auto const size = layer.content().size();
		auto const scaled_size =
		   vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())}
		   * layer.scaleFactor();

		return Utils::axisAlignedBoundingBox(scaled_size, layer.rotation());
	}

	void render(Layer const& layer, Span2d<Pixel> ret);
}

#endif