//@	{
//@	 "targets":[{"name":"patch.test", "type":"application", "autorun":1}]
//@	}

#include "./patch.hpp"

#include "filtergraph/image_processor.hpp"

static_assert(Texpainter::FilterGraph::ImageProcessor<Texpainter::Model::Patch>);

int main() { return 0; }