//@	{
//@	 "targets":[{"name":"image_processor_registry.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_processor_registry.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_IMAGEPROCESSORREGISTRY_HPP
#define TEXPAINTER_IMGPROC_IMAGEPROCESSORREGISTRY_HPP

#include "filtergraph/abstract_image_processor.hpp"

#include <memory>

namespace Texpainter::ImageProcessorRegistry
{
	std::unique_ptr<FilterGraph::AbstractImageProcessor> createImageProcessor(char const* name);

	std::unique_ptr<FilterGraph::AbstractImageProcessor> createImageProcessor(
	    FilterGraph::ImageProcessorId const& id);

	std::span<FilterGraph::ImageProcessorId const> processorIds();
	std::span<char const* const> processorNames();
}

#endif