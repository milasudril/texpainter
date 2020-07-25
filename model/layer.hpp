//@	{
//@	 "targets":[{"name":"layer.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"layer.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_LAYER_HPP
#define TEXPAINTER_MODEL_LAYER_HPP

#include "utils/angle.hpp"
#include "utils/rect.hpp"

#include "./image.hpp"
#include "./brush.hpp"

#include <memory>

namespace Texpainter::Model
{
	class Layer
	{
	public:
		explicit Layer(Image&& img)
		    : m_visible{true}
		    , m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_content{std::make_shared<Image>(std::move(img))}
		{
		}

		explicit Layer(Size2d size, Pixel initial_color = Pixel{0.0f, 0.0f, 0.0f, 0.0f})
		    : m_visible{true}
		    , m_loc{0.0, 0.0}
		    , m_rot{0}
		    , m_scale{1.0, 1.0}
		    , m_content{std::make_shared<Image>(size)}
		{
			fill(initial_color);
		}

		Layer(Layer const& other) = delete;

		~Layer() = default;

		Layer(Layer&&) = default;

		Layer& operator=(Layer&&) = default;

		Layer& operator=(Layer const& other) = delete;

		Layer linkedLayer() const { return Layer{m_visible, m_loc, m_rot, m_scale, m_content}; }

		Layer copiedLayer() const
		{
			return Layer{m_visible, m_loc, m_rot, m_scale, std::make_shared<Image>(*m_content)};
		}

		Layer& convertToCopy()
		{
			m_content = std::make_shared<Image>(*m_content);
			return *this;
		}

		Layer& paint(vec2_t origin, float radius, BrushFunction brush, Pixel color);

		Layer& fill(Pixel color)
		{
			std::ranges::fill(m_content->pixels(), color);
			return *this;
		}

		Image const& content() const { return *m_content; }

		vec2_t location() const { return m_loc; }

		Layer& location(vec2_t loc)
		{
			m_loc = loc;
			return *this;
		}

		Angle rotation() const { return m_rot; }

		Layer& rotation(Angle rot)
		{
			m_rot = rot;
			return *this;
		}

		vec2_t scaleFactor() const { return m_scale; }

		Layer& scaleFactor(vec2_t val)
		{
			m_scale = val;
			return *this;
		}

		bool visible() const { return m_visible; }

		Layer& visible(bool val)
		{
			m_visible = val;
			return *this;
		}

		auto size() const { return m_content->size(); }

	private:
		bool m_visible;
		vec2_t m_loc;
		Angle m_rot;
		vec2_t m_scale;
		std::shared_ptr<Image> m_content;

		explicit Layer(
		    bool vis, vec2_t loc, Angle rot, vec2_t scale, std::shared_ptr<Image> const& content)
		    : m_visible{vis}
		    , m_loc{loc}
		    , m_rot{rot}
		    , m_scale{scale}
		    , m_content{content}
		{
		}
	};

	inline vec2_t axisAlignedBoundingBox(Layer const& layer)
	{
		auto const size = layer.content().size();
		auto const scaled_size =
		    vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())}
		    * layer.scaleFactor();

		return axisAlignedBoundingBox(scaled_size, layer.rotation());
	}

	void render(Layer const& layer, Span2d<Pixel> ret);

	void outline(Layer const& layer, Span2d<Pixel> ret);
}

#endif
