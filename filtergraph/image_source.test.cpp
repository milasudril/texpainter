//@	{
//@	 "targets":[{"name":"image_source.test", "type":"application", "autorun":1}]
//@	}

#include "./image_source.hpp"

#include "./image_processor.hpp"

namespace
{
	static_assert(Texpainter::FilterGraph::ImageProcessor2<
	              Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::RgbaValue>>);
	/*
	template<Texpainter::FilterGraph::ImageProcessor2 ImgProc>
	struct IsImageProcessor{};

	using X = IsImageProcessor<Texpainter::FilterGraph::ImageSource<double>>;
*/

}

int main() { return 0; }