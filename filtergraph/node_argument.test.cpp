//@	{
//@	 "targets":[{"name":"node_argument.test", "type":"application", "autorun":1}]
//@	}

#include "./node_argument.hpp"

#include "./argtuple.hpp"

#include <cassert>
#include <cstring>

constexpr auto types = std::array<Texpainter::FilterGraph::PixelType, 3>{
    Texpainter::FilterGraph::PixelType::GrayscaleReal,
    Texpainter::FilterGraph::PixelType::GrayscaleComplex,
    Texpainter::FilterGraph::PixelType::RGBA,
};

using MyArgTuple = Texpainter::FilterGraph::ArgTuple<types>;

namespace Testcases
{
	void texpainterFilterGraphNodeArgumentGetPointers()
	{
		Texpainter::FilterGraph::RealValue x;
		Texpainter::FilterGraph::ComplexValue y;
		Texpainter::PixelStore::Pixel z;
		Texpainter::Size2d size{23, 56};
		Texpainter::FilterGraph::NodeArgument test{size, {&x, &y, &z}};

		assert(test.size() == size);
		auto vals = test.inputs<MyArgTuple>();
		assert(vals.get<0>() == &x);
		assert(vals.get<1>() == &y);
		assert(vals.get<2>() == &z);
	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeArgumentGetPointers();
	return 0;
}