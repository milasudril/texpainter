//@	{
//@	 "targets":[{"name":"rgba_combine.test", "type":"application", "autorun":1}]
//@	}

#include "./rgba_combine.hpp"

#include "filtergraph/image_processor.hpp"

static_assert(Texpainter::FilterGraph::ImageProcessor2<RgbaCombine::ImageProcessor>);

int main() { return 0; }