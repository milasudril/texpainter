//@	{
//@	 "targets":[{"name":"can_export.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_CANEXPORT_HPP
#define TEXPAINTER_PIXELSTORE_CANEXPORT_HPP

namespace Texpainter::PixelStore
{
	template<class T>
	struct CanExport: std::false_type
	{
	};

	template<class T>
	struct CanExport<std::unique_ptr<T[]>>: CanExport<T>
	{
	};
}

#endif