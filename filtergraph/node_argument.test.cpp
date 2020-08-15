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

using MyInArgTuple  = Texpainter::FilterGraph::InArgTuple<types>;
using MyOutArgTuple = Texpainter::FilterGraph::OutArgTuple<types>;

namespace Testcases
{
	void texpainterFilterGraphNodeArgumentGetPointers()
	{
		Texpainter::FilterGraph::RealValue const a{};
		Texpainter::FilterGraph::ComplexValue const b{};
		Texpainter::PixelStore::Pixel const c{};

		Texpainter::FilterGraph::RealValue x{};
		Texpainter::FilterGraph::ComplexValue y{};
		Texpainter::PixelStore::Pixel z{};

		Texpainter::Size2d size{23, 56};
		Texpainter::FilterGraph::NodeArgument test{size, {&a, &b, &c}, {&x, &y, &z}};

		assert(test.size() == size);

		auto inputs = test.inputs<MyInArgTuple>();
		assert(inputs.get<0>() == &a);
		assert(inputs.get<1>() == &b);
		assert(inputs.get<2>() == &c);

		auto outputs = test.outputs<MyOutArgTuple>();
		assert(outputs.get<0>() == &x);
		assert(outputs.get<1>() == &y);
		assert(outputs.get<2>() == &z);

		*outputs.get<0>() = 1.0;
		assert(*outputs.get<0>() == 1.0);
	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeArgumentGetPointers();
	return 0;
}