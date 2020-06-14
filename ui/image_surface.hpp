//@	{
//@	 "targets":[{"name":"image_surface.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_surface.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_IMAGESURFACE_HPP
#define TEXPAINTER_UI_IMAGESURFACE_HPP

#include "./container.hpp"

#include "geom/dimension.hpp"
#include "model/image.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class ImageSurface
	{
		public:
 			explicit ImageSurface(Container& container);
			~ImageSurface();

			ImageSurface& operator=(ImageSurface&& obj) noexcept
			{
				std::swap(obj.m_impl, m_impl);
				return *this;
			}

			ImageSurface(ImageSurface&& obj) noexcept:m_impl(obj.m_impl)
			{ obj.m_impl=nullptr; }

			template<auto id, class Callback>
			ImageSurface& callback(Callback& cb)
			{
				auto cb_wrapper=[](void* rvc, ImageSurface& self)
				{
					auto x=reinterpret_cast<Callback*>(rvc);
					x->template clicked<id>(self);
				};
				return callback(cb_wrapper, &cb);
			}

			ImageSurface& image(Model::Image const& img);

		private:
			class Impl;
			explicit ImageSurface(Impl& impl):m_impl(&impl){}
			Impl* m_impl;
			using Callback = void (*)(void* cb_obj, ImageSurface& self);
			ImageSurface& callback(Callback cb, void* cb_obj);
	};
}

#endif