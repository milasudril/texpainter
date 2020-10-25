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
		g.insert(
		    Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::PortType::RgbaPixels>{});
	}
}

int main()
{
	Testcases::texpainterFilterGraphGraphInsert();
	return 0;
}