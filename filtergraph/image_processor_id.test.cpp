//@	{
//@	 "targets":[{"name":"image_processor_id.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_id.hpp"

namespace Testcases
{
	void texpainterFiltergraphImageProcessorIdFromToString()
	{
		Texpainter::FilterGraph::ImageProcessorId id_a{"101112131415161718191a1b1c1d1e1f"};
		Texpainter::FilterGraph::ImageProcessorId id_b{"101112131415161718191A1B1C1D1E1F"};

		auto str_a = toString(id_a);
		assert(str_a == "101112131415161718191a1b1c1d1e1f");

		auto str_b = toString(id_b);
		assert(str_b == "101112131415161718191a1b1c1d1e1f");
	}
}

int main()
{
	Testcases::texpainterFiltergraphImageProcessorIdFromToString();
	return 0;
}