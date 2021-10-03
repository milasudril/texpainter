//@	{
//@	 "targets":[{"name":"glarea.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"glarea.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_GLAREA_HPP
#define TEXPAINTER_UI_GLAREA_HPP

#include <utility>

namespace Texpainter::Ui
{
	class Container;

	class GLArea
	{
	public:
		explicit GLArea(Container& cnt);
		~GLArea();

		GLArea& operator=(GLArea&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		GLArea(GLArea&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		GLArea& minHeight(int h);

		GLArea& minWidth(int w);

		GLArea& minSize(int w, int h);

		template<class Callback, class IdType>
		GLArea& callback(Callback& cb_obj, IdType id)
		{
			return callback(Vtable{cb_obj, id}, &cb_obj, static_cast<int>(id));
		}

		int id() const noexcept;

		GLArea& glActivate();

		GLArea& versionRequest(int major, int minor);

		GLArea& redraw() noexcept;

	private:
		class Impl;
		Impl* m_impl;

		GLArea(Impl& impl): m_impl(&impl) {}

		struct Vtable
		{
			Vtable() = default;

			template<class Callback, class IdType>
			explicit Vtable(Callback& cb, IdType) noexcept
			{
				render = [](void* cb_obj, GLArea& source) {
					reinterpret_cast<Callback*>(cb_obj)->render(source,
					                                            static_cast<IdType>(source.id()));
				};
				realize = [](void* cb_obj, GLArea& source) {
					reinterpret_cast<Callback*>(cb_obj)->realize(source,
					                                             static_cast<IdType>(source.id()));
				};
				resize = [](void* cb_obj, GLArea& source, int width, int height) {
					reinterpret_cast<Callback*>(cb_obj)->resize(
					    source, static_cast<IdType>(source.id()), width, height);
				};
			}

			void (*render)(void* cb_obj, GLArea& source);
			void (*realize)(void* cb_obj, GLArea& source);
			void (*resize)(void* cb_obj, GLArea& source, int width, int height);
		};

		GLArea& callback(const Vtable& vt, void* cb_obj, int id);
	};
}

#endif  // UIXX_IMAGEVIEW_HPP
