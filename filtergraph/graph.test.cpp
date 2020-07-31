//@	{
//@	 "targets":[{"name":"graph.test", "type":"application", "autorun":1}]
//@	}

#include "./graph.hpp"
#include "./image_source.hpp"

namespace Testcases
{
	void texpainterFilterGraphGraphInsert()
	{
		Texpainter::FilterGraph::Graph g;
		Texpainter::PixelStore::Image img{1, 1};
		g.insert(Texpainter::FilterGraph::ImageSource<Texpainter::PixelStore::Pixel>{img.pixels()});
	}
}

int main()
{
	Testcases::texpainterFilterGraphGraphInsert();
	return 0;
}