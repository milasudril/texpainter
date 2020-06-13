//@	{"targets":[{"name":"container.hpp","type":"include"}]}

#ifndef TEXPAINTER_UI_CONTAINER_HPP
#define TEXPAINTER_UI_CONTAINER_HPP

namespace Texpainter::Ui
{
	class Container
	{
		public:
			virtual ~Container() = default;
			virtual Container& add(void* handle) = 0;
			virtual Container& show() = 0;
			virtual Container& sensitive(bool val) = 0;
			virtual void* toplevel() const = 0;
	};
}

#endif
