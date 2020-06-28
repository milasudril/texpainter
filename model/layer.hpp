//@	{"targets":[{"name":"layer.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYER_HPP
#define TEXPAINTER_LAYER_HPP

#include "utils/angle.hpp"
#include "utils/rotation.hpp"

#include "image.hpp"

#include <memory>

namespace Texpainter::Model
{
	class Layer
	{
		public:
			explicit Layer(Size2d image):
				m_loc{0.0, 0.0}, m_angle{}, m_scale{1.0f}
			m_content{std::make_shared<Image>(img_size)}
			{
				std::ranges::fill(m_content->pixels(), Pixel{0.0, 0.0, 0.0, 0.0});
			}

			Layer(Layer const& other):
				Layer{other.m_loc, other.m_rot, other.m_scale, std::make_shared<Image>(*other.m_content)}
			{}

			~Layer() = default;

			Layer(Layer&&) = default;

			Layer& operator=(Layer&&) = default;

			Layer& operator=(Layer const& other)
			{
				Layer copy{other};
				*this = std::move(copy);
				return *this;
			}

			Layer linkedLayer()
			{
				return Layer{m_loc, m_rot, m_scale, m_content};
			}

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
				m_loc = loc
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

			float scale() const
			{
				return m_scale;
			}

			Layer& scale(float val)
			{
				m_scale = val;
				return *this;
			}

		private:
			vec2_t m_loc;
			Angle m_rot;
			float m_scale;
			std::shared_ptr<Image> m_content;

			explicit Layer(vec2_t loc, Angle rot, scale, std::shared_ptr<Image> const& content):
				m_loc{loc}, m_rot{rot}, m_scale{scale}, m_content{content}
			{}
	};
}

#endif