//@	{"targets":[{"name":"resource_pool.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_RESOURCEPOOL_HPP
#define TEXPAINTER_MODEL_RESOURCEPOOL_HPP

#include "utils/resource_manager.hpp"

namespace Texpainter::Model
{
	class ResourcePool: private ResourceManager<PixelStore::Image>
	{
	public:
		using ResourceManager<PixelStore::Image>::create;
		using ResourceManager<PixelStore::Image>::incUsecount;
		using ResourceManager<PixelStore::Image>::decUsecount;
		using ResourceManager<PixelStore::Image>::get;

	private:
	};
}

#endif
