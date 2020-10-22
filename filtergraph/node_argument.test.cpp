//@	{
//@	 "targets":[{"name":"node_argument.test", "type":"application", "autorun":1}]
//@	}

#include "./node_argument.hpp"

#include "./argtuple.hpp"

#include <cassert>
#include <cstring>

constexpr auto types = std::array<Texpainter::FilterGraph::PortType, 3>{
    Texpainter::FilterGraph::PortType::GrayscaleReal,
    Texpainter::FilterGraph::PortType::GrayscaleComplex,
    Texpainter::FilterGraph::PortType::RGBA,
};

using MyInArgTuple = Texpainter::FilterGraph::InArgTuple<types>;

namespace Testcases
{
	void texpainterFilterGraphNodeArgumentGetPointers()
	{
		Texpainter::FilterGraph::RealValue const a{};
		Texpainter::FilterGraph::ComplexValue const b{};
		Texpainter::PixelStore::Pixel const c{};

		Texpainter::Size2d size{23, 56};
		Texpainter::FilterGraph::NodeArgument test{size, {&a, &b, &c}};

		assert(test.size() == size);

		auto inputs = test.inputs<MyInArgTuple>();
		assert(inputs.get<0>() == &a);
		assert(inputs.get<1>() == &b);
		assert(inputs.get<2>() == &c);
	}
}

int main()
{
	Testcases::texpainterFilterGraphNodeArgumentGetPointers();
	return 0;
}